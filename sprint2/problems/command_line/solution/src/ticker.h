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

    Ticker(Strand strand, std::chrono::milliseconds period, Handler handler 
    ) 
        : strand_{ strand }
        , period_{ period }
        , handler_{ handler }
    {
        last_tick_ = chrono::steady_clock::now();
    };

    void Start() {
        last_tick_ = chrono::steady_clock::now();
        /* Выполнить SchedulTick внутри strand_ */
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
    void OnTick(sys::error_code ec) {
        if (!ec)
        {
            auto current_tick = chrono::steady_clock::now();
            handler_(chrono::duration_cast<chrono::milliseconds>(current_tick - last_tick_));
            last_tick_ = current_tick;
            ScheduleTick();
        }
    }

    Strand strand_;
    net::steady_timer timer_{ strand_ };
    std::chrono::milliseconds period_;
    Handler handler_;
    chrono::steady_clock::time_point last_tick_;
};