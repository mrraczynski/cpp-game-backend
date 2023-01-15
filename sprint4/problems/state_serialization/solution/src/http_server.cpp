#include "http_server.h"

namespace http_server {
    void SessionBase::Run() {
        // Вызываем метод Read, используя executor объекта stream_.
        // Таким образом вся работа со stream_ будет выполняться, используя его executor
        net::dispatch(stream_.get_executor(),
            beast::bind_front_handler(&SessionBase::Read, GetSharedThis()));
    }

    void SessionBase::Read()
    {
        using namespace std::literals;
        // Очищаем запрос от прежнего значения (метод Read может быть вызван несколько раз)
        request_ = {};
        stream_.expires_after(30s);
        // Считываем request_ из stream_, используя buffer_ для хранения считанных данных
        http::async_read(stream_, buffer_, request_,
            // По окончании операции будет вызван метод OnRead
            beast::bind_front_handler(&SessionBase::OnRead, GetSharedThis()));
        begin = chrono::steady_clock::now();
    }

    void SessionBase::OnRead(beast::error_code ec, [[maybe_unused]] std::size_t bytes_read) {
        using namespace std::literals;
        if (ec == http::error::end_of_stream) {
            // Нормальная ситуация - клиент закрыл соединение
            return Close();
        }
        if (ec) {
            //std::cerr << "read"sv << ": " << ec.message() << std::endl;
            BOOST_LOG_TRIVIAL(info) << boost::log::add_value(logger::additional_data, boost::json::value(
                {
                    {"code", ec.value()},
                    {"text", ec.message()},
                    {"where", "read"}
                })) << "error"sv;
            return;
        }
        BOOST_LOG_TRIVIAL(info) << boost::log::add_value(logger::additional_data, boost::json::value(
            {
                {"ip", stream_.socket().remote_endpoint().address().to_string()},
                {"URI", request_.target()},
                {"method", request_.method_string()}
            })) << "request received"sv;
        HandleRequest(std::move(request_));
    }

    void SessionBase::Close() {
        beast::error_code ec;
        stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
    }

    void SessionBase::OnWrite(bool close, beast::error_code ec, [[maybe_unused]] std::size_t bytes_written) {
        if (ec) {
            //std::cerr << std::string_view("write") << ": " << ec.message() << std::endl;
            BOOST_LOG_TRIVIAL(info) << boost::log::add_value(logger::additional_data, boost::json::value(
                {
                    {"code", ec.value()},
                    {"text", ec.message()},
                    {"where", "write"}
                })) << "error"sv;
            return;
        }

        if (close) {
            // Семантика ответа требует закрыть соединение
            return Close();
        }

        // Считываем следующий запрос
        Read();
    }

// Разместите здесь реализацию http-сервера, взяв её из задания по разработке асинхронного сервера

}  // namespace http_server
