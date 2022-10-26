#pragma once
#include "http_server.h"
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
            std::vector<std::string_view> target_vec = SplitRequest(req.target());
            if (req.method() != http::verb::get)
            {
                send(ResponseNotAllowed(req));
                return;
            }
            else if (!IsApiRequest(target_vec))
            {
                send(ResponseError(req, http::status::bad_request, "badRequest", "Bad request")); //TODO: new answer if needed
                return;
            }
            else if (!IsGoodRequest(target_vec))
            {
                send(ResponseError(req, http::status::bad_request, "badRequest", "Bad request"));
                return;
            }
            else if (!HasMapID(target_vec))
            {
                send(ResponseAllMaps(req));
            }
            else
            {
                const model::Map* map = game_.FindMap(model::Map::Id(std::string(target_vec[4])));
                if (map != nullptr)
                {
                    send(ResponseMapById(req, map));
                }
                else
                {
                    send(ResponseError(req, http::status::not_found, "mapNotFound", "Map not found"));
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
    std::vector<std::string_view> SplitRequest(const std::string_view target);

    template <typename Body, typename Allocator>
    StringResponse ResponseMapById(http::request<Body, http::basic_fields<Allocator>> req, const model::Map* map)
    {
        StringResponse response = StringResponse(http::status::ok, req.version());
        response.set(http::field::content_type, ContentType::APPLICATION_JSON);
        json_loader::GetMapJson(response.body(), map);
        response.content_length(response.body().size());
        return response;
    }

    template <typename Body, typename Allocator>
    StringResponse ResponseNotAllowed(http::request<Body, http::basic_fields<Allocator>> req)
    {
        StringResponse response = StringResponse(http::status::method_not_allowed, req.version());
        return response;
    }

    template <typename Body, typename Allocator>
    StringResponse ResponseAllMaps(http::request<Body, http::basic_fields<Allocator>> req)
    {
        StringResponse response = StringResponse(http::status::ok, req.version());
        response.set(http::field::content_type, ContentType::APPLICATION_JSON);
        json_loader::GetMapsJson(response.body(), game_);
        response.content_length(response.body().size());
        return response;
    }

    template <typename Body, typename Allocator>
    StringResponse ResponseError(http::request<Body, http::basic_fields<Allocator>> req, const http::status& status_code, const std::string& code, const std::string& message)
    {
        StringResponse response = StringResponse(status_code, req.version());
        response.set(http::field::content_type, ContentType::APPLICATION_JSON);
        json_loader::GetErrorJson(response.body(), code, message);
        response.content_length(response.body().size());
        return response;
    }

    bool IsApiRequest(const std::vector<std::string_view>& target_vec);

    bool IsGoodRequest(const std::vector<std::string_view>& target_vec);

    bool HasMapID(const std::vector<std::string_view>& target_vec);

    const model::Map* FindMapID(const std::vector<std::string_view>& target_vec);

    model::Game& game_;
};

}  // namespace http_handler
