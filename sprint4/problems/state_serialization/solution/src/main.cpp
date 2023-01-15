#include "sdk.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <thread>

#include "request_handler.h"
#include "model_serialization.h"

using namespace std::literals;
namespace net = boost::asio;
namespace sys = boost::system;
namespace fs = std::filesystem;
namespace ser = serialization;

namespace {

struct Args {
    std::optional<int> tick_period;
    std::string config_file;
    std::string www_root;
    bool is_randomize_spawn_points = false;
    std::optional<std::string> state_file = std::nullopt;
    std::optional<int> save_state_period = std::nullopt;
}; 

[[nodiscard]] std::optional<Args> ParseCommandLine(int argc, const char* const argv[]) {

    namespace po = boost::program_options;

    po::options_description desc{ "Allowed options"s };
    Args args;
    boost::optional<int> tick_period_int;
    desc.add_options()           
        ("help,h", "produce help message") 
        ("tick-period,t", po::value(&tick_period_int)->multitoken()->value_name("milliseconds"s), "set tick period")
        ("config-file,c", po::value(&args.config_file)->value_name("config file"s), "set config file") 
        ("www-root,w", po::value(&args.www_root)->value_name("dir"s), "set static files root")
        ("randomize-spawn-points", "spawn dogs at random positions")
        ("state-file,s", po::value(&args.state_file.value())->value_name("state file"s), "set state file")
        ("save-state-period,s", po::value(&args.save_state_period.value())->value_name("save state period"s), "set save state period")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    
    if (tick_period_int)
    {
        args.tick_period = tick_period_int.value();
    }
    else
    {
        args.tick_period = std::nullopt;
    }

    if (vm.contains("help"s)) {
        std::cout << desc;
        return std::nullopt;
    }

    if (vm.contains("randomize-spawn-points"))
    {
        args.is_randomize_spawn_points = true;
    }

    if (!vm.contains("config-file"s)) {
        throw std::runtime_error("Configuration file is not specified"s);
    }

    if (!vm.contains("www-root"s)) {
        throw std::runtime_error("Static files root is not specified"s);
    }

    return args;

}

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

bool file_exists(const fs::path& p, fs::file_status s = fs::file_status{})
{
    if (fs::status_known(s) ? fs::exists(s) : fs::exists(p))
        return true;
    else
        return false;
}

void LoadSaveFile(model::Game& game, std::optional<Args>& args, std::optional<std::function<void()>>& save_func, fs::path& st_file)
{
    if (args.value().state_file != std::nullopt)
    {
        st_file = fs::path{ args.value().state_file.value() };

        if (file_exists(st_file))
        {
            std::ifstream file(st_file);
            std::stringstream ss;
            ss << file.rdbuf();
            file.close();

            boost::archive::text_iarchive ia{ ss };
            try
            {
                ser::GameRepr game_repr;
                ia >> game_repr;
                game = game_repr.Restore();
            }
            catch (std::exception& ex)
            {
                BOOST_LOG_TRIVIAL(info) << boost::log::add_value(logger::additional_data, boost::json::value(
                    {
                        {"code", "EXIT_FAILURE"},
                        {"exception", ex.what()}
                    })) << "save file read error"sv;
                throw ex;
            }

        }
        else
        {
            // 1. Загружаем карту из файла и построить модель игры
            game = json_loader::LoadGame(args.value().config_file,
                args.value().is_randomize_spawn_points);
        }
        save_func = [&]() {
            try
            {
                std::string path = st_file.string() + ".tmp"s;
                std::ofstream file(path);
                std::stringstream ss;
                boost::archive::text_oarchive oa{ ss };
                ser::GameRepr game_repr{ game };
                oa << game_repr;
                file << ss.rdbuf();
                file.close();
                fs::rename(path, path.substr(0, path.find(".tmp"s)));
            }
            catch (std::exception& ex)
            {
                BOOST_LOG_TRIVIAL(info) << boost::log::add_value(logger::additional_data, boost::json::value(
                    {
                        {"code", -1},
                        {"exception", ex.what()}
                    })) << "save file write error"sv;
                return;
            }
        };
    }
    else
    {
        // 1. Загружаем карту из файла и построить модель игры
        game = json_loader::LoadGame(args.value().config_file,
            args.value().is_randomize_spawn_points);
    }
}

int main(int argc, const char* argv[]) {
    try {
        logger::InitBoostLog();
        if (auto args = ParseCommandLine(argc, argv)) {
            model::Game game;
            std::optional<std::function<void()>> save_func = std::nullopt;
            fs::path st_file;

            LoadSaveFile(game, args, save_func, st_file);

            // 2. Инициализируем io_context
            const unsigned num_threads = std::thread::hardware_concurrency();
            net::io_context ioc(num_threads);

            // 3. Добавляем асинхронный обработчик сигналов SIGINT и SIGTERM
            net::signal_set signals(ioc, SIGINT, SIGTERM);
            signals.async_wait([&ioc](const sys::error_code& ec, [[maybe_unused]] int signal_number) {
                if (!ec) {
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
                args.value().www_root,
                ioc,
                args.value().tick_period,
                save_func,
                args.value().save_state_period
                );

            // 5. Запустить обработчик HTTP-запросов, делегируя их обработчику запросов
            const auto address = net::ip::make_address("0.0.0.0");
            constexpr unsigned short port = 8080;
            http_server::ServeHttp(ioc, { address, port }, [&handler](auto&& req, auto&& send) {
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
        }
        return EXIT_SUCCESS;
    } catch (const std::exception& ex) {
        BOOST_LOG_TRIVIAL(info) << boost::log::add_value(logger::additional_data, boost::json::value(
            {
                {"code", "EXIT_FAILURE"},
                {"exception", ex.what()}
            })) << "server exited"sv;
        return EXIT_FAILURE;
    }
}
