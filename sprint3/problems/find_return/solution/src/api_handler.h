#pragma once
#include "ticker.h"

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

    // ��������� ContentType ����� ������� ��������� ��� ��������,
    // �������� �������� HTTP-��������� Content-Type
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
        constexpr static std::string_view MODEL_FBX = "model/obj"sv;
        constexpr static std::string_view MODEL_OBJ = "model/obj"sv;
        constexpr static std::string_view WEBMANIFEST = "application/manifest+json"sv;
        // ��� ������������� ������ ContentType ����� �������� � ������ ���� ��������
    };
   
    class ApiHandler {
    public:
        explicit ApiHandler(model::Game& game)
            : game_{ game } {
        }

        ApiHandler& operator=(const ApiHandler&) = delete;  

        template <typename Body, typename Allocator, typename Send>
        void HandleAPIRequest(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send, const std::vector<std::string>& target_vec, bool is_accepting_tick)
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
                else if (IsPlayerActionRequest(target_vec))
                {
                    send(HandlePlayerActionRequest(req));
                    return;
                }
                else if (IsTimeTickRequest(target_vec))
                {
                    send(HandleTimeTickRequest(req, is_accepting_tick));
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
            if (req.method() != http::verb::head)
            {
                json_loader::GetMapJson(response.body(), map);
            }
            response.content_length(response.body().size());
            return response;
        }

        template <typename Body, typename Allocator>
        StringResponse ResponseAllMaps(http::request<Body, http::basic_fields<Allocator>> req)
        {
            StringResponse response = StringResponse(http::status::ok, req.version());
            response.set(http::field::content_type, ContentType::APPLICATION_JSON);
            if (req.method() != http::verb::head)
            {
                json_loader::GetMapsJson(response.body(), game_);
            }
            response.content_length(response.body().size());
            return response;
        }

        template <typename Body, typename Allocator>
        StringResponse ResponseError(http::request<Body, http::basic_fields<Allocator>> req, const std::string_view& content_type, 
            const http::status& status_code, const std::string& code, const std::string& message, const std::string_view& cache_control = "no-cache"sv)
        {
            StringResponse response = StringResponse(status_code, req.version());
            response.set(http::field::content_type, content_type);
            json_loader::GetErrorJson(response.body(), code, message);
            response.content_length(response.body().size());
            response.set(http::field::cache_control, cache_control);
            return response;
        }

        template <typename Body, typename Allocator>
        StringResponse ResponsePostRequest(const http::request<Body, http::basic_fields<Allocator>>& req, std::string& body_str,
            const http::status& status_code, const std::string_view& content_type, 
            const std::string_view& cache_control = "no-cache"sv, const std::string_view& allowed = "POST"sv)
        {
            StringResponse response = StringResponse(status_code, req.version());
            response.set(http::field::content_type, content_type);
            response.set(http::field::cache_control, cache_control);
            if (status_code == http::status::method_not_allowed)
            {
                response.set(http::field::allow, allowed);
            }
            if (req.method() != http::verb::head)
            {
                response.body() = std::move(body_str);
            }
            response.prepare_payload();
            return response;
        }

        std::optional<std::string> TryExtractToken(const std::string_view& bearer_token);

        template <typename Body, typename Allocator>
        StringResponse MakeUnauthorizedError(const http::request<Body, http::basic_fields<Allocator>>& req)
        {
            std::string body_str;
            json_loader::GetErrorJson(body_str, "invalidToken", "Authorization header is missing");
            return ResponsePostRequest(req, body_str, http::status::unauthorized, ContentType::APPLICATION_JSON, "no-cache"sv);
        }

        template <typename Fn, typename Body, typename Allocator>
        StringResponse ExecuteAuthorized(Fn&& action, const http::request<Body, http::basic_fields<Allocator>>& req) {
            try
            {
                std::string_view bearer_token = req["Authorization"];
                if (auto token = TryExtractToken(bearer_token)) {
                    return action(token.value());
                }
                else {
                    return MakeUnauthorizedError(req);
                }
            }
            catch (std::exception& e)
            {
                BOOST_LOG_TRIVIAL(info) << boost::log::add_value(logger::additional_data, boost::json::value(
                    {
                        {"code", -1},
                        {"text", e.what()},
                        {"where", "api_handler/ExecuteAuthorized"}
                    })) << "error"sv;
            }
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
                //TODO: �������� ������ ����������� ������
                model::Player player(id, user_name, game_.FindMap(model::Map::Id(map_id))->GetRandomPointOnRoad().value(), game_.FindGameSessionByMap(model::Map::Id(map_id)));
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

            return ExecuteAuthorized([&](const std::string_view& bearer_token) {
                const model::Player* player = model::PlayerTokens::GetInstance().FindPlayerByToken(model::Token(std::string(bearer_token)));
                if (player == nullptr)
                {
                    std::string body_str;
                    json_loader::GetErrorJson(body_str, "unknownToken", "Player token has not been found");
                    return ResponsePostRequest(req, body_str, http::status::unauthorized, ContentType::APPLICATION_JSON, "no-cache"sv);
                }
                std::vector<model::Player> session_players = model::PlayerTokens::GetInstance().GetPlayersBySession(player->GetSession());

                std::string body_str = json_loader::CreatePlayersArray(session_players);
                return ResponsePostRequest(req, body_str, http::status::ok, ContentType::APPLICATION_JSON, "no-cache"sv);
                }, req);                        
        }

        template <typename Body, typename Allocator>
        StringResponse HandleTimeTickRequest(const http::request<Body, http::basic_fields<Allocator>>& req, bool is_accepting_tick)
        {
            if (!is_accepting_tick)
            {
                std::string body_str;
                json_loader::GetErrorJson(body_str, "badRequest", "Invalid endpoint");
                return ResponsePostRequest(req, body_str, http::status::bad_request, ContentType::APPLICATION_JSON, "no-cache"sv, "POST"sv);
            }

            if (req.method() != http::verb::post)
            {
                std::string body_str;
                json_loader::GetErrorJson(body_str, "invalidMethod", "Invalid method");
                return ResponsePostRequest(req, body_str, http::status::method_not_allowed, ContentType::APPLICATION_JSON, "no-cache"sv);
            }

            json::value body_json;
            json::object obj;
            double time_delta;
            try
            {
                body_json = json::parse(req.body());
                std::string s = json::serialize(body_json);
                obj = body_json.as_object();
                std::string s1 = json::serialize(obj);
                int time_delta_int = obj["timeDelta"].as_int64();
                time_delta = time_delta_int;
            }
            catch (std::exception& e)
            {
                std::string body_str;
                json_loader::GetErrorJson(body_str, "invalidArgument", "Failed to parse action");
                return ResponsePostRequest(req, body_str, http::status::bad_request, ContentType::APPLICATION_JSON, "no-cache"sv);
            }
            game_.TickGame(time_delta);
            std::string body_str = json_loader::GetEmptyObject();
            return ResponsePostRequest(req, body_str, http::status::ok, ContentType::APPLICATION_JSON, "no-cache"sv);
        }

        template <typename Body, typename Allocator>
        StringResponse HandlePlayerActionRequest(const http::request<Body, http::basic_fields<Allocator>>& req)
        {
            if (req.method() != http::verb::post)
            {
                std::string body_str;
                json_loader::GetErrorJson(body_str, "invalidMethod", "Invalid method");
                return ResponsePostRequest(req, body_str, http::status::method_not_allowed, ContentType::APPLICATION_JSON, "no-cache"sv, "POST"sv);
            }

            return ExecuteAuthorized([&](const std::string_view& bearer_token) {
                json::value body_json;
                json::object obj;
                std::string_view move_dir;
                try
                {
                    body_json = json::parse(req.body());
                    obj = body_json.as_object();
                    move_dir = obj["move"].as_string().data();
                }
                catch (std::exception& e)
                {
                    std::string body_str;
                    json_loader::GetErrorJson(body_str, "invalidArgument", "Failed to parse action");
                    return ResponsePostRequest(req, body_str, http::status::bad_request, ContentType::APPLICATION_JSON, "no-cache"sv);
                }
                std::string_view cont_type = req[http::field::content_type];
                if (cont_type.size() == 0 || cont_type.compare(ContentType::APPLICATION_JSON) != 0)
                {
                    std::string body_str;
                    json_loader::GetErrorJson(body_str, "invalidArgument", "Invalid content type");
                    return ResponsePostRequest(req, body_str, http::status::bad_request, ContentType::APPLICATION_JSON, "no-cache"sv);
                }
                game_.SetPlayerDir(bearer_token, move_dir);
                std::string body_str = json_loader::GetEmptyObject();
                return ResponsePostRequest(req, body_str, http::status::ok, ContentType::APPLICATION_JSON, "no-cache"sv);
                }, req);
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

            return ExecuteAuthorized([&](const std::string_view& bearer_token) {
                std::string ec;
                std::string cur_state = game_.GetCurrentGameState(bearer_token, ec);
                if (ec.find("unknownToken") != std::string::npos)
                {
                    std::string body_str;
                    json_loader::GetErrorJson(body_str, "unknownToken", "Player token has not been found");
                    return ResponsePostRequest(req, body_str, http::status::unauthorized, ContentType::APPLICATION_JSON, "no-cache"sv);
                }
                return ResponsePostRequest(req, cur_state, http::status::ok, ContentType::APPLICATION_JSON, "no-cache"sv);
                }, req);            
        }

        template <typename Body, typename Allocator>
        StringResponse HandleMapsRequest(http::request<Body, http::basic_fields<Allocator>>& req, const std::vector<std::string>& target_vec)
        {
            if (req.method() != http::verb::get && req.method() != http::verb::head)
            {
                std::string body_str;
                json_loader::GetErrorJson(body_str, "invalidMethod", "Invalid method");
                return ResponsePostRequest(req, body_str, http::status::method_not_allowed, ContentType::APPLICATION_JSON, "no-cache"sv, "GET, HEAD"sv);
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

        bool IsApiRequest(const std::vector<std::string>& target_vec);

        bool IsMapsRequest(const std::vector<std::string>& target_vec);

        bool IsGameRequest(const std::vector<std::string>& target_vec);

        bool IsGameJoinRequest(const std::vector<std::string>& target_vec);

        bool IsGamePlayersRequest(const std::vector<std::string>& target_vec);

        bool IsGameStateRequest(const std::vector<std::string>& target_vec);

        bool IsPlayerActionRequest(const std::vector<std::string>& target_vec);

        bool IsTimeTickRequest(const std::vector<std::string>& target_vec);

        bool HasMapID(const std::vector<std::string>& target_vec);

        const model::Map* FindMapID(const std::vector<std::string>& target_vec);

        model::Game& game_;

    };

}  // namespace http_handler