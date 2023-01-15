#pragma once

#include <memory>
#include <filesystem>
#include <cassert>
#include <functional>
#include <iostream>

#include "http_server.h"
#include "json_loader.h"

#define BOOST_BEAST_USE_STD_STRING_VIEW

namespace sys = boost::system;
namespace net = boost::asio;
namespace chrono = std::chrono;

class Ticker : public std::enable_shared_from_this<Ticker> {

public:

    using Strand = net::strand<net::io_context::executor_type>&;
    using Handler = std::function<void(std::chrono::milliseconds delta)>;
    using Saver = std::function<void()>;

    Ticker(Strand strand, std::chrono::milliseconds period, Handler handler, std::optional<Saver> saver = std::nullopt, std::optional<std::chrono::milliseconds> save_period = std::nullopt
    ) 
        : strand_{ strand }
        , period_{ period }
        , handler_{ handler }
        , saver_{ saver }
        , save_period_{ save_period }
    {
        last_tick_ = chrono::steady_clock::now();
        last_save_ = chrono::steady_clock::now();
    };

    void Start() {
        last_tick_ = chrono::steady_clock::now();
        last_save_ = chrono::steady_clock::now();
        /* Выполнить SchedulTick внутри strand_ */
        net::dispatch(strand_, [self = this->shared_from_this()] {
            self->ScheduleSave();
            });
        return net::dispatch(strand_, [self = this->shared_from_this()] {
            self->ScheduleTick();
            });
    }
private:
    void ScheduleTick() {
        /* выполнить OnTick через промежуток времени period_ */
        timer_.expires_after(period_);
        timer_.async_wait([self = this->shared_from_this()](sys::error_code ec) {
            self->OnTick(ec);
            });
    }
    void ScheduleSave() {
        /* выполнить OnTick через промежуток времени period_ */
        if (save_period_ == std::nullopt)
        {
            timer_.expires_after(period_);
        }
        else
        {
            timer_.expires_after(save_period_.value());
        }
        timer_.async_wait([self = this->shared_from_this()](sys::error_code ec) {
            self->OnSave(ec);
            });
    }
    void OnTick(sys::error_code ec) {
        if (!ec)
        {
            auto current_tick = chrono::steady_clock::now();
            handler_(chrono::duration_cast<chrono::milliseconds>(current_tick - last_tick_));
            last_tick_ = current_tick;
            ScheduleTick();
        }
    }
    void OnSave(sys::error_code ec) {
        if (!ec && saver_ != std::nullopt)
        {
            auto current_save = chrono::steady_clock::now();
            auto& func = saver_.value();
            func();
            last_save_ = current_save;
            ScheduleSave();
        }
    }

    Strand strand_;
    net::steady_timer timer_{ strand_ };
    std::chrono::milliseconds period_;
    std::optional<std::chrono::milliseconds> save_period_;
    Handler handler_;
    std::optional <Saver> saver_;
    chrono::steady_clock::time_point last_tick_;
    chrono::steady_clock::time_point last_save_;
};