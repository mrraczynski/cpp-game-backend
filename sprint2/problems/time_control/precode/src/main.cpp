#include "sdk.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <iostream>
#include <thread>

#include "request_handler.h"

using namespace std::literals;
namespace net = boost::asio;
namespace sys = boost::system;

namespace {

// Запускает функцию fn на n потоках, включая текущий
template <typename Fn>
void RunWorkers(unsigned n, const Fn& fn) {
    n = std::max(1u, n);
    std::vector<std::jthread> workers;
    workers.reserve(n - 1);
    // Запускаем n-1 рабочих потоков, выполняющих функцию fn
    while (--n) {
        workers.emplace_back(fn);
    }
    fn();
}

}  // namespace

int main(int argc, const char* argv[]) {
    /*if (argc != 3) {
        std::cerr << "Usage: game_server <game-config-json> <files-folder-path>"sv << std::endl;
        return EXIT_FAILURE;
    }*/
    try {
        logger::InitBoostLog();

        // 1. Загружаем карту из файла и построить модель игры
        model::Game game = json_loader::LoadGame("E:/GitHub/cpp-game-backend/sprint2/problems/move_players/precode/data/config.json"/*argv[1]*/ );

        // 2. Инициализируем io_context
        const unsigned num_threads = std::thread::hardware_concurrency();
        net::io_context ioc(num_threads);

        // 3. Добавляем асинхронный обработчик сигналов SIGINT и SIGTERM
        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const sys::error_code& ec, [[maybe_unused]] int signal_number) {
            if (!ec) {
                //std::cout << "Signal "sv << signal_number << " received"sv << std::endl;
                BOOST_LOG_TRIVIAL(info) << boost::log::add_value(logger::additional_data, boost::json::value(
                    {
                        {"code", 0},
                        {"exception", nullptr}
                    })) << "server exited"sv;
                ioc.stop();
            }
            });

        // 4. Создаём обработчик HTTP-запросов и связываем его с моделью игры
        std::shared_ptr<http_handler::RequestHandler> handler = std::make_shared<http_handler::RequestHandler>(game, 
            "E:/GitHub/cpp-game-backend/sprint2/problems/move_players/precode/static/"/* argv[2]*/, 
            ioc);

        // 5. Запустить обработчик HTTP-запросов, делегируя их обработчику запросов
        const auto address = net::ip::make_address("0.0.0.0");
        constexpr unsigned short port = 8080;
        http_server::ServeHttp(ioc, {address, port}, [&handler](auto&& req, auto&& send) {
            //logging_handler(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
            (*handler)(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
        });
        

        // Эта надпись сообщает тестам о том, что сервер запущен и готов обрабатывать запросы
        BOOST_LOG_TRIVIAL(info) << boost::log::add_value(logger::additional_data, boost::json::value(
            {
                {"port", port},
                {"address", address.to_string()}
            })) << "Server has started...";

        // 6. Запускаем обработку асинхронных операций
        RunWorkers(std::max(1u, num_threads), [&ioc] {
            ioc.run();
        });
    } catch (const std::exception& ex) {
        //std::cerr << ex.what() << std::endl;
        BOOST_LOG_TRIVIAL(info) << boost::log::add_value(logger::additional_data, boost::json::value(
            {
                {"code", "EXIT_FAILURE"},
                {"exception", ex.what()}
            })) << "server exited"sv;
        return EXIT_FAILURE;
    }
}
