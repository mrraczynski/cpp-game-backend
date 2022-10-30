#pragma once
#include <filesystem>
#include <cassert>
#include "http_server.h"
#include "json_loader.h"


#define BOOST_BEAST_USE_STD_STRING_VIEW

namespace http_handler {
namespace beast = boost::beast;
namespace http = beast::http;
namespace sys = boost::system;

using namespace std::literals;
namespace fs = std::filesystem;

using StringResponse = http::response<http::string_body>;
using FileResponse = http::response<http::file_body>;
using StringRequest = http::request<http::string_body>;

// Структура ContentType задаёт область видимости для констант,
// задающий значения HTTP-заголовка Content-Type
struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TEXT_HTML = "text/html"sv;
    constexpr static std::string_view APPLICATION_JSON = "application/json"sv;
    constexpr static std::string_view TEXT_CSS = "text/css"sv;
    constexpr static std::string_view TEXT_PLAIN = "text/plain"sv;
    constexpr static std::string_view TEXT_JAVASCRIPT = "text/javascript"sv;
    constexpr static std::string_view APPLICATION_XML = "application/xml"sv;
    constexpr static std::string_view IMAGE_PNG = "image/png"sv;
    constexpr static std::string_view IMAGE_JPEG = "image/jpeg"sv;
    constexpr static std::string_view IMAGE_GIF = "image/gif"sv;
    constexpr static std::string_view IMAGE_BMP = "image/bmp"sv;
    constexpr static std::string_view IMAGE_ICO = "image/vnd.microsoft.icon"sv;
    constexpr static std::string_view IMAGE_TIFF = "image/tiff"sv;
    constexpr static std::string_view IMAGE_SVG = "image/svg + xml"sv;
    constexpr static std::string_view AUDIO_MPEG = "audio/mpeg"sv;
    constexpr static std::string_view FOLDER = "folder"sv;
    // При необходимости внутрь ContentType можно добавить и другие типы контента
};

class RequestHandler {
public:
    explicit RequestHandler(model::Game& game, fs::path static_dir)
        : game_{ game }, static_dir_{static_dir} {
    }

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
        // Обработать запрос request и отправить ответ, используя send
        try
        {
            std::string decoded_path = UrlDecode(req.target());
            std::vector<std::string_view> target_vec = SplitRequest(decoded_path);
            if (req.method() != http::verb::get)
            {
                send(ResponseNotAllowed(req));
                return;
            }
            else if (!IsApiRequest(target_vec))
            {              
                HandleStaticFileRequest(std::move(req), std::move(send), decoded_path);
                return;
            }
            else
            {
                HandleAPIRequest(std::move(req), std::move(send), target_vec);
                return;
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

    template <typename Body, typename Allocator>
    FileResponse ResponseFile(http::request<Body, http::basic_fields<Allocator>> req, http::file_body::value_type& file, const std::string_view& content_type)
    {
        FileResponse response = FileResponse(http::status::ok, req.version());
        response.set(http::field::content_type, content_type);
        response.body() = std::move(file);
        response.prepare_payload();
        return response;
    }

    template <typename Body, typename Allocator, typename Send>
    void HandleAPIRequest(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send, const std::vector<std::string_view>& target_vec)
    {
        if (!IsGoodRequest(target_vec))
        {
            send(ResponseError(req, http::status::bad_request, "badRequest", "Bad request"));
            return;
        }
        else
        {
            if (!HasMapID(target_vec))
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
    }

    template <typename Body, typename Allocator, typename Send>
    void HandleStaticFileRequest(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send, const std::string& decoded_path)
    {
        fs::path absolute_path(static_dir_ / decoded_path.substr(1));
        if (IsSubPath(absolute_path, static_dir_))
        {
            std::pair<bool, const std::string_view&> content_type = GetFileContentType(decoded_path); 
            if (!content_type.first)
            {
                absolute_path = absolute_path / "index.html"sv;
            }

            http::file_body::value_type file;
            if (sys::error_code ec; file.open(absolute_path.string().data(), beast::file_mode::read, ec), ec)
            {
                send(ResponseError(req, http::status::not_found, "fileNotFound", "File not found"));
            }
            else
            {
                send(ResponseFile(req, file, content_type.second));
            }
        }
        else
        {
            send(ResponseError(req, http::status::bad_request, "badRequest", "Bad request"));
        }
    }

    bool IsApiRequest(const std::vector<std::string_view>& target_vec);

    bool IsGoodRequest(const std::vector<std::string_view>& target_vec); 
    
    bool IsSubPath(fs::path path, fs::path base);

    bool HasMapID(const std::vector<std::string_view>& target_vec);

    const model::Map* FindMapID(const std::vector<std::string_view>& target_vec);

    std::string UrlDecode(const std::string_view& value);

    std::pair<bool, const std::string_view&> GetFileContentType(const std::string& decoded_path);

    model::Game& game_;

    fs::path static_dir_;
};

}  // namespace http_handler
