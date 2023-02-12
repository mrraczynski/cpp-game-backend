#pragma once
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "api_handler.h"

namespace http_handler {

class RequestHandler : public std::enable_shared_from_this<RequestHandler> {
public:
    explicit RequestHandler(model::Game& game, const postgres::Database& db, fs::path static_dir, net::io_context& ioc, std::optional<int> tick_period,
        std::optional<std::function<void()>> save_func, std::optional<int> save_state_period = std::nullopt)
        : game_{ game }, static_dir_{ static_dir }, ioc_{ ioc }, save_func_{ save_func }, db_ { db }
    {
        if (tick_period)
        {
            std::function<void(std::chrono::milliseconds)> tick_func = [&](std::chrono::milliseconds delta_time) {
                TickerHandler(delta_time);
            };
            std::optional<std::chrono::milliseconds> save_state_period_mil;
            if (save_state_period != std::nullopt)
            {
                save_state_period_mil = std::chrono::milliseconds(save_state_period.value());
            }
            ticker_ptr = std::make_shared<Ticker>(strand_, std::chrono::milliseconds(tick_period.value()), tick_func, 
                save_func, save_state_period_mil);
            ticker_ptr->Start();
        }
        else
        {            
            accept_tick_request = true;
        }
    }

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
        // Обработать запрос request и отправить ответ, используя send
        try
        {
            std::string decoded_path = UrlDecode(req.target());
            target_vec = std::vector<std::string>();
            target_vec = SplitRequest(decoded_path);
            if (!IsApiRequest(target_vec))
            {           
                HandleStaticFileRequest(std::move(req), std::move(send), decoded_path);
                return;
            }
            else
            {   
                std::shared_ptr<ApiHandler> api_handler = api_handler_;                // Все запросы к API выполняются последовательно внутри strand
                api_handler->HandleAPIRequest(std::move(req), std::move(send), db_, target_vec, accept_tick_request, save_func_);
            }
        }
        catch (std::exception& e)
        {
            send(ResponseError(req, ContentType::APPLICATION_JSON, http::status::internal_server_error, "internalServerError", "Internal server error"));
        }
    }

private:
    std::vector<std::string> SplitRequest(const std::string target);

    template <typename Body, typename Allocator>
    StringResponse ResponseNotAllowed(http::request<Body, http::basic_fields<Allocator>> req)
    {
        StringResponse response = StringResponse(http::status::method_not_allowed, req.version());
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

    template <typename Body, typename Allocator>
    StringResponse ResponseError(http::request<Body, http::basic_fields<Allocator>> req, const std::string_view& content_type, const http::status& status_code, const std::string& code, const std::string& message)
    {
        StringResponse response = StringResponse(status_code, req.version());
        response.set(http::field::content_type, content_type);
        json_loader::GetErrorJson(response.body(), code, message);
        response.content_length(response.body().size());
        return response;
    } 

    template <typename Body, typename Allocator, typename Send>
    void HandleStaticFileRequest(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send, const std::string& decoded_path)
    {
        if (req.method() != http::verb::get)
        {
            send(ResponseNotAllowed(req));
            return;
        }
        fs::path absolute_path(static_dir_ / decoded_path.substr(1));
        if (IsSubPath(absolute_path, static_dir_))
        {
            std::pair<bool, std::string_view> content_type = GetFileContentType(decoded_path);
            if (!content_type.first)
            {
                absolute_path = absolute_path / "index.html"sv;
                content_type.second = ContentType::TEXT_HTML;
            }

            http::file_body::value_type file;
            if (sys::error_code ec; file.open(absolute_path.string().data(), beast::file_mode::read, ec), ec)
            {
                send(ResponseError(req, ContentType::TEXT_PLAIN, http::status::not_found, "fileNotFound", "File not found"));
                return;
            }
            else
            {
                send(ResponseFile(req, file, content_type.second));
                return;
            }
        }
        else
        {
            send(ResponseError(req, ContentType::TEXT_PLAIN, http::status::bad_request, "badRequest", "Bad request"));
            return;
        }
    }

    void TickerHandler(std::chrono::milliseconds delta_time)
    {
        game_.TickGame(delta_time.count());
    }

    bool IsApiRequest(const std::vector<std::string>& target_vec);
    
    bool IsSubPath(fs::path path, fs::path base);

    std::string UrlDecode(const std::string_view& value);

    std::pair<bool, std::string_view> GetFileContentType(const std::string& decoded_path);

    model::Game& game_;

    fs::path static_dir_;

    net::io_context& ioc_;

    net::strand<net::io_context::executor_type> strand_{ net::make_strand(ioc_) };

    std::shared_ptr<ApiHandler> api_handler_ {std::make_shared<ApiHandler>(game_)};

    std::shared_ptr<Ticker> ticker_ptr{ nullptr };

    bool accept_tick_request = false;

    std::vector<std::string> target_vec;

    std::optional<std::function<void()>> save_func_;

    const postgres::Database& db_;

};

}  // namespace http_handler