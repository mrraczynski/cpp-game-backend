#ifdef WIN32
#include <sdkddkver.h>
#endif

#include "seabattle.h"

#include <atomic>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <optional>
#include <string>
#include <thread>
#include <string_view>

namespace net = boost::asio;
using net::ip::tcp;
using namespace std::literals;

void PrintFieldPair(const SeabattleField& left, const SeabattleField& right) {
    auto left_pad = "  "s;
    auto delimeter = "    "s;
    std::cout << left_pad;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << delimeter;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << std::endl;
    for (size_t i = 0; i < SeabattleField::field_size; ++i) {
        std::cout << left_pad;
        left.PrintLine(std::cout, i);
        std::cout << delimeter;
        right.PrintLine(std::cout, i);
        std::cout << std::endl;
    }
    std::cout << left_pad;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << delimeter;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << std::endl;
}

template <size_t sz>
static std::optional<std::string> ReadExact(tcp::socket& socket) {
    boost::array<char, sz> buf;
    boost::system::error_code ec;

    net::read(socket, net::buffer(buf), net::transfer_exactly(sz), ec);

    if (ec) {
        return std::nullopt;
    }

    return {{buf.data(), sz}};
}

static bool WriteExact(tcp::socket& socket, std::string_view data) {
    boost::system::error_code ec;

    net::write(socket, net::buffer(data), net::transfer_exactly(data.size()), ec);

    return !ec;
}

class SeabattleAgent {
public:
    SeabattleAgent(const SeabattleField& field)
        : my_field_(field) {
    }

    void StartGame(tcp::socket& socket, bool my_initiative) {
        net::streambuf stream_buf;
        size_t size;
        std::string my_data, download_info;
        boost::system::error_code ec;
        PrintFieldPair(this->my_field_, this->other_field_);
        while (true)
        {
            if (my_initiative)
            {
                std::optional<std::pair<int, int>> move = SendMove(socket);
                SeabattleField::ShotResult res = ReadResult(socket, move);
                switch (res)
                {
                case SeabattleField::ShotResult::MISS:
                    std::cout << "Miss!"sv << std::endl;
                    my_initiative = false;
                    break;
                case SeabattleField::ShotResult::HIT:
                    std::cout << "Hit!"sv << std::endl;
                    break;
                case SeabattleField::ShotResult::KILL:
                    std::cout << "Kill!"sv << std::endl;
                    break;
                }                
                PrintFieldPair(this->my_field_, this->other_field_);
            }
            else
            {
                std::optional<std::pair<int, int>> move = ReadMove(socket);
                SeabattleField::ShotResult res = SendResult(socket, move);
                if (res == SeabattleField::ShotResult::MISS)
                {
                    my_initiative = true;
                }                
                PrintFieldPair(this->my_field_, this->other_field_);
            }            
        }
    }

private:
    static std::optional<std::pair<int, int>> ParseMove(const std::string_view& sv) {
        if (sv.size() != 2) return std::nullopt;

        int p1 = sv[0] - 'A', p2 = sv[1] - '1';

        if (p1 < 0 || p1 > 8) return std::nullopt;
        if (p2 < 0 || p2 > 8) return std::nullopt;

        return {{p1, p2}};
    }

    static std::string MoveToString(std::pair<int, int> move) {
        char buff[] = {static_cast<char>(move.first) + 'A', static_cast<char>(move.second) + '1'};
        return {buff, 2};
    }

    void PrintFields() const {
        PrintFieldPair(my_field_, other_field_);
    }

    bool IsGameEnded() const {
        return my_field_.IsLoser() || other_field_.IsLoser();
    }

    std::optional <std::pair<int, int>> SendMove(tcp::socket& socket)
    {        
        std::string str_move;
        std::cout << "Your turn: "sv;
        std::cin >> str_move;
        socket.write_some(net::buffer(str_move));
        return ParseMove(str_move);
    }
    SeabattleField::ShotResult SendResult(tcp::socket& socket, std::optional <std::pair<int, int>> move)
    {
        SeabattleField::ShotResult res = my_field_.Shoot(move.value().second, move.value().first);
        socket.write_some(net::buffer(std::to_string(static_cast<int> (res))));
        return res;
    }
    std::optional <std::pair<int, int>> ReadMove(tcp::socket& socket)
    {
        std::cout << "Waiting for turn..."sv << std::endl;
        boost::system::error_code ec;
        net::streambuf stream_buf(2);
        net::read(socket, stream_buf, ec);
        std::string str_move{ std::istreambuf_iterator<char>(&stream_buf),
                            std::istreambuf_iterator<char>() };
        if (ec) {
            std::cout << "Error reading data"sv << std::endl;
            std::cout << ec.message() << std::endl;
        }
        std::cout << "Shot to "sv << str_move << std::endl;
        return ParseMove(str_move);
    }
    SeabattleField::ShotResult ReadResult(tcp::socket& socket, std::optional <std::pair<int, int>> move)
    {
        boost::system::error_code ec;
        net::streambuf stream_buf(1);
        net::read(socket, stream_buf, ec);
        std::string str_res{ std::istreambuf_iterator<char>(&stream_buf),
                            std::istreambuf_iterator<char>() };
        if (ec) {
            std::cout << "Error reading data"sv << std::endl;
            std::cout << ec.message() << std::endl;
        }
        int res = std::stoi(str_res);
        switch (res)
        {
        case 0:
            other_field_.MarkMiss(move.value().second, move.value().first);
            return SeabattleField::ShotResult::MISS;
            break;
        case 1:
            other_field_.MarkHit(move.value().second, move.value().first);
            return SeabattleField::ShotResult::HIT;
            break;
        case 2:
            other_field_.MarkKill(move.value().second, move.value().first);
            return SeabattleField::ShotResult::KILL;
            break;
        }
    }

private:
    SeabattleField my_field_;
    SeabattleField other_field_;
};

void StartServer(const SeabattleField& field, unsigned short port) {
    SeabattleAgent agent(field);
    net::io_context io_context;

    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
    std::cout << "Waiting for connection..."sv << std::endl;

    boost::system::error_code ec;
    tcp::socket socket{ io_context };
    acceptor.accept(socket, ec);

    if (ec) {
        std::cout << "Can't accept connection"sv << std::endl;
        return;
    }

    agent.StartGame(socket, false);
};

void StartClient(const SeabattleField& field, const std::string& ip_str, unsigned short port) {
    SeabattleAgent agent(field);

    boost::system::error_code ec;
    auto endpoint = tcp::endpoint(net::ip::make_address(ip_str, ec), port);

    if (ec) {
        std::cout << "Wrong IP format"sv << std::endl;
        return;
    }

    net::io_context io_context;
    tcp::socket socket{ io_context };
    socket.connect(endpoint, ec);

    if (ec) {
        std::cout << "Can't connect to server"sv << std::endl;
        return;
    }

    agent.StartGame(socket, true);
};

/*int main(int argc, const char** argv) {
    std::string i = "1111";
    std::mt19937 engine(std::stoi(i));
    SeabattleField fieldL = SeabattleField::GetRandomField(engine);

    //StartServer(fieldL, 3333);
    StartClient(fieldL, std::string("127.0.0.1"sv), 3333);

    if (argc == 3) {
        StartServer(fieldL, std::stoi(argv[2]));
    } else if (argc == 4) {
        StartClient(fieldL, argv[2], std::stoi(argv[3]));
    }
}*/

int main(int argc, const char** argv) {
    if (argc != 3 && argc != 4) {
        std::cout << "Usage: program <seed> [<ip>] <port>" << std::endl;
        return 1;
    }
    std::mt19937 engine(std::stoi(argv[1]));
    SeabattleField fieldL = SeabattleField::GetRandomField(engine);

    if (argc == 3) {
        StartServer(fieldL, std::stoi(argv[2]));
    }
    else if (argc == 4) {
        StartClient(fieldL, argv[2], std::stoi(argv[3]));
    }
}
