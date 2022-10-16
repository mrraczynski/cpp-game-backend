#ifdef WIN32
#include <sdkddkver.h>
#endif
// boost.beast будет использовать std::string_view вместо boost::string_view
#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <thread>
#include <optional>

namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace std::literals;
namespace beast = boost::beast;
namespace http = beast::http;

// Boost.Beast будет использовать std::string_view вместо boost::string_view
#define BOOST_BEAST_USE_STD_STRING_VIEW

// Подключаем заголовочный файл <sdkddkver.h> в системе Windows,
// чтобы избежать предупреждения о неизвестной версии Platform SDK,
// когда используем заголовочные файлы библиотеки Boost.Asio
#ifdef WIN32
#include <sdkddkver.h>
#endif

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <iostream> 
#include <thread>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace std::literals;

namespace beast = boost::beast;
namespace http = beast::http;
// Запрос, тело которого представлено в виде строки
using StringRequest = http::request<http::string_body>;
// Ответ, тело которого представлено в виде строки
using StringResponse = http::response<http::string_body>;

// Структура ContentType задаёт область видимости для констант,
// задающий значения HTTP-заголовка Content-Type
struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TEXT_HTML = "text/html"sv;
    // При необходимости внутрь ContentType можно добавить и другие типы контента
};

enum RequestMethod
{
    GET,
    HEAD,
    ANY
};

// Создаёт StringResponse с заданными параметрами
StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
    bool keep_alive, RequestMethod method,
    std::string_view content_type = ContentType::TEXT_HTML) 
{
    StringResponse response(status, http_version);
    response.set(http::field::content_type, content_type);
    if (method == RequestMethod::GET)
    {
        response.body() = body;
        response.content_length(body.size());
    }
    else if (method == RequestMethod::HEAD)
    {
        response.body() = ""sv;
        response.content_length(body.size());
    }
    else
    {
        response.body() = "Invalid method"sv;
        response.set("Allow"sv, "GET, HEAD"sv);
        response.content_length(14);
    }
    response.keep_alive(keep_alive);
    return response;
}

StringResponse HandleRequest(StringRequest&& req) {
    RequestMethod method;
    http::status status;
    std::string body;
    std::string_view content_type;
    body = "Hello, "s.append(req.target().substr(1));
    if (req.method_string() == "GET"sv)
    {
        method = RequestMethod::GET;
        status = http::status::ok;
        content_type = ContentType::TEXT_HTML;
    }
    else if (req.method_string() == "HEAD"sv)
    {
        method = RequestMethod::HEAD;
        status = http::status::ok;
        content_type = ContentType::TEXT_HTML;
    }
    else
    {
        method = RequestMethod::ANY;
        status = http::status::method_not_allowed;
        content_type = ContentType::TEXT_HTML;        
    }

    const auto text_response = [&req](http::status status, std::string_view text, RequestMethod method, std::string_view content_type) {
        return MakeStringResponse(status, text, req.version(), req.keep_alive(), method, content_type);
    };

    // Здесь можно обработать запрос и сформировать ответ, но пока всегда отвечаем: Hello
    return text_response(status, std::string_view(body), method, content_type);
}

void DumpRequest(const StringRequest& req) {
    std::cout << req.method_string() << ' ' << req.target() << std::endl;
    // Выводим заголовки запроса
    for (const auto& header : req) {
        std::cout << "  "sv << header.name_string() << ": "sv << header.value() << std::endl;
    }
}

std::optional<StringRequest> ReadRequest(tcp::socket& socket, beast::flat_buffer& buffer) {
    beast::error_code ec;
    StringRequest req;
    // Считываем из socket запрос req, используя buffer для хранения данных.
    // В ec функция запишет код ошибки.
    http::read(socket, buffer, req, ec);

    if (ec == http::error::end_of_stream) {
        return std::nullopt;
    }
    if (ec) {
        throw std::runtime_error("Failed to read request: "s.append(ec.message()));
    }
    return req;
}

template <typename RequestHandler>
void HandleConnection(tcp::socket& socket, RequestHandler&& handle_request) {
    try {
        // Буфер для чтения данных в рамках текущей сессии.
        beast::flat_buffer buffer;

        // Продолжаем обработку запросов, пока клиент их отправляет
        while (auto request = ReadRequest(socket, buffer)) {
            //DumpRequest(*request);
            StringResponse response = handle_request(*std::move(request));
            http::write(socket, response);
            if (response.need_eof()) {
                break;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    beast::error_code ec;
    // Запрещаем дальнейшую отправку данных через сокет
    socket.shutdown(tcp::socket::shutdown_send, ec);
}

int main() {
    net::io_context ioc;
    const auto address = net::ip::make_address("0.0.0.0");
    constexpr unsigned short port = 8080;

    tcp::acceptor acceptor(ioc, { address, port });
    std::cout << "Server has started..."sv;
    while (true) {
        tcp::socket socket(ioc);
        acceptor.accept(socket);

        // Запускаем обработку взаимодействия с клиентом в отдельном потоке
        std::thread t(
            // Лямбда-функция будет выполняться в отдельном потоке
            [](tcp::socket socket) {
                HandleConnection(socket, HandleRequest);
            },
            std::move(socket));  // Сокет нельзя скопировать, но можно переместить

        // После вызова detach поток продолжит выполняться независимо от объекта t
        t.detach();
    }
}

/*int main() {
    // Выведите строчку "Server has started...", когда сервер будет готов принимать подключения
}*/
