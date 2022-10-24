#pragma once
#include "http_server.h"
#include "model.h"
#include "json_loader.h"

namespace http_handler {
namespace beast = boost::beast;
namespace http = beast::http;

using namespace std::literals;

using StringResponse = http::response<http::string_body>;
using StringRequest = http::request<http::string_body>;

// Структура ContentType задаёт область видимости для констант,
// задающий значения HTTP-заголовка Content-Type
struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TEXT_HTML = "text/html"sv;
    constexpr static std::string_view APPLICATION_JSON = "application/json"sv;
    // При необходимости внутрь ContentType можно добавить и другие типы контента
};

class RequestHandler {
public:
    explicit RequestHandler(model::Game& game)
        : game_{game} {
    }

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    // Создаёт StringResponse с заданными параметрами
    StringResponse MakeStringResponse(const http::request<http::string_body>& req, http::status status, std::string_view body, unsigned http_version,
        bool keep_alive, std::string_view content_type = ContentType::TEXT_HTML)
    {  
        StringResponse response(status, http_version);
        response.result(http::status::ok);
        response.set(http::field::content_type, content_type);
        if (req.method() == http::verb::get)
        {
            response.body() = body;
            response.content_length(body.size());
        }
        else if (req.method() == http::verb::head)
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

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
        // Обработать запрос request и отправить ответ, используя send
        StringResponse response;
        http::status status;
        std::string body;
        std::string_view content_type;
        if (req.method() == http::verb::get)
        {            
            std::string_view sv ("/api/v1/maps/"sv);
            if (req.target().compare("/api/v1/maps"sv) == 0)
            {
                response = StringResponse(http::status::ok, req.version());
                json_loader::GetMapsJson(response.body(), game_);
                response.content_length(response.body().size());
            }
            else if (req.target().find(sv) != std::string::npos)
            {
                std::string tg(req.target());
                tg.erase(0, sv.length());
                response = StringResponse(http::status::ok, req.version());
                json_loader::GetMapJsonById(response.body(), game_, tg);
                response.content_length(response.body().size());
            }
            //status = http::status::ok;
            //content_type = ContentType::TEXT_HTML;
            //response.result(http::status::ok);
            //response.set(http::field::content_type, content_type);
            //response.body() = body;
            //response.content_length(body.size());
        }
        else if (req.method() == http::verb::head)
        {           
            status = http::status::ok;
            content_type = ContentType::TEXT_HTML;
        }
        else
        {
            status = http::status::method_not_allowed;
            content_type = ContentType::TEXT_HTML;
        }
        send(response);
        /*const auto text_response = [&](http::status status, std::string_view text, RequestMethod method, std::string_view content_type) {
            return MakeStringResponse(status, text, req.version(), req.keep_alive(), method, content_type);
        };*/

        // Здесь можно обработать запрос и сформировать ответ, но пока всегда отвечаем: Hello
        //send = text_response(status, std::string_view(body), method, content_type);
    }

private:
    model::Game& game_;

};

}  // namespace http_handler
