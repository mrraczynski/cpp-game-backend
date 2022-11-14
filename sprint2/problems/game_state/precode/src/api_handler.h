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
    namespace net = boost::asio;

    using namespace std::literals;
    namespace fs = std::filesystem;

    using StringResponse = http::response<http::string_body>;
    using FileResponse = http::response<http::file_body>;
    using StringRequest = http::request<http::string_body>;
    template<typename ResponseType>
    using Response = http::response<ResponseType>;

    // —труктура ContentType задаЄт область видимости дл€ констант,
    // задающий значени€ HTTP-заголовка Content-Type
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
        constexpr static std::string_view IMAGE_SVG = "image/svg+xml"sv;
        constexpr static std::string_view AUDIO_MPEG = "audio/mpeg"sv;
        constexpr static std::string_view FOLDER = "folder"sv;
        // ѕри необходимости внутрь ContentType можно добавить и другие типы контента
    };
   
    class ApiHandler {
    public:
        explicit ApiHandler(model::Game& game)
            : game_{ game } {
        }

        //RequestHandler(const RequestHandler&) = delete;
        ApiHandler& operator=(const ApiHandler&) = delete;  

        template <typename Body, typename Allocator, typename Send>
        //StringResponse HandleAPIRequest(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send, const std::vector<std::string_view>& target_vec)
        void HandleAPIRequest(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send, const std::vector<std::string_view>& target_vec)
        {
            try {
                if (IsGameJoinRequest(target_vec))
                {
                    send(HandleJoinGameRequest(req));
                    return;
                }
                else if (IsGamePlayersRequest(target_vec))
                {
                    send(HandlePlayersRequest(req));
                    return;
                }
                else if (IsGameStateRequest(target_vec))
                {
                    send(HandleStateRequest(req));
                    return;
                }
                else
                {
                    send(HandleMapsRequest(req, target_vec));
                    return;
                }
            }
            catch (std::exception& e)
            {
                /*std::string body_str;
                json_loader::GetErrorJson(body_str, e.what(), e.what());
                send(ResponsePostRequest(req, body_str, http::status::unauthorized, ContentType::APPLICATION_JSON, e.what()));*/
                send(ResponseError(req, ContentType::APPLICATION_JSON, http::status::internal_server_error, "internalServerError", e.what()));
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
        StringResponse ResponseAllMaps(http::request<Body, http::basic_fields<Allocator>> req)
        {
            StringResponse response = StringResponse(http::status::ok, req.version());
            response.set(http::field::content_type, ContentType::APPLICATION_JSON);
            json_loader::GetMapsJson(response.body(), game_);
            response.content_length(response.body().size());
            return response;
        }

        template <typename Body, typename Allocator>
        StringResponse ResponseError(http::request<Body, http::basic_fields<Allocator>> req, const std::string_view& content_type, const http::status& status_code, const std::string& code, const std::string& message)
        {
            StringResponse response = StringResponse(status_code, req.version());
            response.set(http::field::content_type, content_type);
            json_loader::GetErrorJson(response.body(), code, message);
            response.content_length(response.body().size());
            return response;
        }

        template <typename Body, typename Allocator>
        StringResponse ResponsePostRequest(const http::request<Body, http::basic_fields<Allocator>>& req, std::string& body_str, const http::status& status_code,
            const std::string_view& content_type, const std::string_view& cache_control, const std::string_view& allowed = "POST"sv)
        {
            StringResponse response = StringResponse(status_code, req.version());
            response.set(http::field::content_type, ContentType::APPLICATION_JSON);
            response.set(http::field::cache_control, "no-cache"sv);
            if (status_code == http::status::method_not_allowed)
            {
                response.set(http::field::allow, allowed);
            }
            response.body() = std::move(body_str);
            response.prepare_payload();
            return response;
        }

        template <typename Body, typename Allocator>
        StringResponse HandleJoinGameRequest(const http::request<Body, http::basic_fields<Allocator>>& req)
        {
            if (req.method() != http::verb::post)
            {
                std::string body_str;
                json_loader::GetErrorJson(body_str, "invalidMethod", "Only POST method is expected");
                return ResponsePostRequest(req, body_str, http::status::method_not_allowed, ContentType::APPLICATION_JSON, "no-cache"sv, "POST");
            }
            json::value body_json;
            json::object obj;
            std::string user_name;
            std::string map_id;
            try
            {
                body_json = json::parse(req.body());
                obj = body_json.as_object();
                user_name = obj["userName"].as_string().data();
                map_id = obj["mapId"].as_string().data();
            }
            catch (std::exception& e)
            {
                std::string body_str;
                json_loader::GetErrorJson(body_str, "invalidArgument", "Join game request parse error");
                return ResponsePostRequest(req, body_str, http::status::bad_request, ContentType::APPLICATION_JSON, "no-cache"sv);
            }
            if (user_name.size() == 0)
            {
                std::string body_str;
                json_loader::GetErrorJson(body_str, "invalidArgument", "Invalid name");
                return ResponsePostRequest(req, body_str, http::status::bad_request, ContentType::APPLICATION_JSON, "no-cache"sv);
            }
            else if (map_id.size() == 0 || game_.FindMap(model::Map::Id(map_id)) == nullptr)
            {
                std::string body_str;
                json_loader::GetErrorJson(body_str, "mapNotFound", "Map not found");
                return ResponsePostRequest(req, body_str, http::status::not_found, ContentType::APPLICATION_JSON, "no-cache"sv);
            }
            else
            {
                int id = game_.GetPlayerId();
                //TODO: изменить логику определени€ сессии

                model::Player player(id, user_name, game_.FindMap(model::Map::Id(map_id))->GetRandomPointOnRoad(), game_.FindGameSessionByMap(model::Map::Id(map_id)));
                model::Token token = model::PlayerTokens::GetInstance().AddPlayer(player);
                std::string body_str;
                json_loader::GetAuthInfo(body_str, *token, id);
                return ResponsePostRequest(req, body_str, http::status::ok, ContentType::APPLICATION_JSON, "no-cache"sv);
            }
        }

        template <typename Body, typename Allocator>
        StringResponse HandlePlayersRequest(const http::request<Body, http::basic_fields<Allocator>>& req)
        {
            if (req.method() != http::verb::get && req.method() != http::verb::head)
            {
                std::string body_str;
                json_loader::GetErrorJson(body_str, "invalidMethod", "Invalid method");
                return ResponsePostRequest(req, body_str, http::status::method_not_allowed, ContentType::APPLICATION_JSON, "no-cache"sv, "GET, HEAD");
            }

            std::string_view bearer_token = req["Authorization"];
            if (bearer_token.size() < 8 || bearer_token.find("Bearer") == std::string::npos)
            {
                std::string body_str;
                json_loader::GetErrorJson(body_str, "invalidToken", "Authorization header is missing");
                return ResponsePostRequest(req, body_str, http::status::unauthorized, ContentType::APPLICATION_JSON, "no-cache"sv);
            }
            std::string token = std::string(bearer_token.substr(7, bearer_token.size()));
            const model::Player* player = model::PlayerTokens::GetInstance().FindPlayerByToken(model::Token(token));
            if (player == nullptr)
            {
                std::string body_str;
                json_loader::GetErrorJson(body_str, "unknownToken", "Player token has not been found");
                return ResponsePostRequest(req, body_str, http::status::unauthorized, ContentType::APPLICATION_JSON, "no-cache"sv);
            }
            std::vector<model::Player> session_players = model::PlayerTokens::GetInstance().GetPlayersBySession(player->GetSession());

            std::string body_str = json_loader::CreatePlayersArray(session_players);
            return ResponsePostRequest(req, body_str, http::status::ok, ContentType::APPLICATION_JSON, "no-cache"sv);
        }

        template <typename Body, typename Allocator>
        StringResponse HandleStateRequest(const http::request<Body, http::basic_fields<Allocator>>& req)
        {
            if (req.method() != http::verb::get && req.method() != http::verb::head)
            {
                std::string body_str;
                json_loader::GetErrorJson(body_str, "invalidMethod", "Invalid method");
                return ResponsePostRequest(req, body_str, http::status::method_not_allowed, ContentType::APPLICATION_JSON, "no-cache"sv, "GET, HEAD"sv);
            }

            std::string_view bearer_token = req["Authorization"];
            if (bearer_token.size() != 39 || bearer_token.find("Bearer") == std::string::npos)
            {
                std::string body_str;
                json_loader::GetErrorJson(body_str, "invalidToken", "Authorization header is required");
                return ResponsePostRequest(req, body_str, http::status::unauthorized, ContentType::APPLICATION_JSON, "no-cache"sv);
            }
            std::string ec;
            std::string cur_state = game_.GetCurrentGameState(bearer_token.substr(7, bearer_token.size()), ec);
            if (ec.find("unknownToken") != std::string::npos)
            {
                std::string body_str;
                json_loader::GetErrorJson(body_str, "unknownToken", "Player token has not been found");
                return ResponsePostRequest(req, body_str, http::status::unauthorized, ContentType::APPLICATION_JSON, "no-cache"sv);
            }
            return ResponsePostRequest(req, cur_state, http::status::ok, ContentType::APPLICATION_JSON, "no-cache"sv);
        }

        template <typename Body, typename Allocator>
        StringResponse HandleMapsRequest(http::request<Body, http::basic_fields<Allocator>>& req, const std::vector<std::string_view>& target_vec)
        {
            if (req.method() != http::verb::get)
            {
                std::string body_str;
                json_loader::GetErrorJson(body_str, "invalidMethod", "Invalid method");
                return ResponsePostRequest(req, body_str, http::status::method_not_allowed, ContentType::APPLICATION_JSON, "no-cache"sv, "GET");
            }

            if (!IsMapsRequest(target_vec))
            {
                return ResponseError(req, ContentType::APPLICATION_JSON, http::status::bad_request, "badRequest", "Bad request");
            }
            else
            {
                if (!HasMapID(target_vec))
                {
                    return ResponseAllMaps(req);
                }
                else
                {
                    const model::Map* map = game_.FindMap(model::Map::Id(std::string(target_vec[4])));
                    if (map != nullptr)
                    {
                        return ResponseMapById(req, map);
                    }
                    else
                    {
                        return ResponseError(req, ContentType::APPLICATION_JSON, http::status::not_found, "mapNotFound", "Map not found");
                    }
                }
            }
        }

        bool IsApiRequest(const std::vector<std::string_view>& target_vec);

        bool IsMapsRequest(const std::vector<std::string_view>& target_vec);

        bool IsGameRequest(const std::vector<std::string_view>& target_vec);

        bool IsGameJoinRequest(const std::vector<std::string_view>& target_vec);

        bool IsGamePlayersRequest(const std::vector<std::string_view>& target_vec);

        bool IsGameStateRequest(const std::vector<std::string_view>& target_vec);

        bool HasMapID(const std::vector<std::string_view>& target_vec);

        const model::Map* FindMapID(const std::vector<std::string_view>& target_vec);

        model::Game& game_;

    };

}  // namespace http_handler
