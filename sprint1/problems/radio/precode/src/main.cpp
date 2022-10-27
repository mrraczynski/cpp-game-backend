#include <boost/asio.hpp>
#include "audio.h"
#include <iostream>
#include <string>
#include <string_view>

namespace net = boost::asio;
using net::ip::udp;

using namespace std::literals;

void StartClient(uint16_t port)
{
    const size_t max_datagram_size = 65507;
    try {
        boost::asio::io_context io_context;
        std::string ip_addr;
        Recorder recorder(ma_format_u8, 1);
        std::cout << "Please enter server ip address:" << std::endl;
        std::cin >> ip_addr;

        auto rec_result = recorder.Record(max_datagram_size, 1.5s);

        size_t size = rec_result.frames * recorder.GetFrameSize();

        udp::socket socket(io_context, udp::v4());
        boost::system::error_code ec;
        auto endpoint = udp::endpoint(net::ip::address_v4::from_string(ip_addr, ec), port);
        socket.send_to(boost::asio::buffer(std::move(rec_result.data), size), endpoint);

        std::cout << "Server responded "sv << std::endl;
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void StartServer(uint16_t port)
{
    static const size_t max_buffer_size = 65507;

    try {
        boost::asio::io_context io_context;

        udp::socket socket(io_context, udp::endpoint(udp::v4(), port));

        for (;;) {
            std::vector<char> recv_buf(max_buffer_size);
            udp::endpoint remote_endpoint;
            std::cout << "Starting to receive..."sv << std::endl;

            auto size = socket.receive_from(boost::asio::buffer(recv_buf), remote_endpoint);

            std::cout << "Data received"sv << std::endl;

            Player player(ma_format_u8, 1);
            size_t buf_size = recv_buf.size() / player.GetFrameSize();
            player.PlayBuffer(std::move(recv_buf.data()), buf_size, 1.5s);
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

int main(int argc, char** argv) {
    while (true)
    {
        std::string mode;
        std::cout << "Chose mode: type \"client\" for client mode and \"server\" for server mode:"sv << std::endl;
        std::cin >> mode;
        if (mode.compare("client") == 0)
        {
            std::cout << "Please enter port number "sv << std::endl;
            uint16_t port;
            std::cin >> port;
            StartClient(port);
        }
        else if (mode.compare("server") == 0)
        {
            std::cout << "Please enter port number "sv << std::endl;
            uint16_t port;
            std::cin >> port;
            StartServer(port);
        }
        else
        {
            std::cout << "Wrong input"sv << std::endl;
        }
    }
    return 0;
}
