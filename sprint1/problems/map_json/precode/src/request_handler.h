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

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
        // Обработать запрос request и отправить ответ, используя send
        try
        {
            StringResponse response;
            if (req.method() == http::verb::get)
            {
                std::string api = std::string(req.target());
                api.erase(5, api.size());
                std::string_view sv("/api/v1/maps/"sv);
                if (req.target().compare("/api/v1/maps"sv) == 0)
                {
                    response = StringResponse(http::status::ok, req.version());
                    json_loader::GetMapsJson(response.body(), game_);
                    response.content_length(response.body().size());
                    send(response);
                }
                else if (req.target().find(sv) != std::string::npos)
                {
                    std::string tg(req.target());
                    tg.erase(0, sv.length());
                    if (game_.FindMap(model::Map::Id(tg)) != nullptr)
                    {
                        response = StringResponse(http::status::ok, req.version());
                        json_loader::GetMapJsonById(response.body(), game_, tg);
                        response.content_length(response.body().size());
                        send(response);
                    }
                    else
                    {
                        response = StringResponse(http::status::not_found, req.version());
                        json_loader::GetErrorJson(response.body(), "mapNotFound", "Map not found");
                        response.content_length(response.body().size());
                        send(response);
                    }
                }
                else if (api.compare("/api/"sv) == 0)
                {
                    response = StringResponse(http::status::not_found, req.version());
                    json_loader::GetErrorJson(response.body(), "badRequest", "Bad request");
                    response.content_length(response.body().size());
                    send(response);
                }
            }
        }
        catch (std::exception& e)
        {
            std::cout << "request_handler.h/operator(): " << e.what() << std::endl;
            return;
        }
    }

private:
    model::Game& game_;

};

}  // namespace http_handler
