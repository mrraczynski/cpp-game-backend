#pragma once
#include <pqxx/pqxx>
#include <boost/signals2/signal.hpp>
#include <boost/bind/bind.hpp>
#include <memory>

#include "connection_pool.h"
#include "model.h"

namespace postgres {

using Wrapper = conn::ConnectionPool::ConnectionWrapper;

class PlayerRepositoryImpl : public model::PlayerRepository, public std::enable_shared_from_this<PlayerRepositoryImpl> {
public:
    explicit PlayerRepositoryImpl(conn::ConnectionPool& pool)
        : pool_(pool) {}

    void SaveProgress(const model::Player& player) override;
    std::vector<model::PlayerRepositoryAttributes> GetProgress(int start, int max_items) const override;

    void SetSignals()
    {
        model::PlayerTokens::GetInstance().sig.connect([self = this->shared_from_this()](const model::Player& player) {
            self->SaveProgress(player);
            });
    }

private:
    conn::ConnectionPool& pool_;
};

class Database {
public:
    explicit Database(const unsigned pool_size, std::string db_url);

    std::vector<model::PlayerRepositoryAttributes> GetProgress(int start, int max_items) const
    {
        return players_->GetProgress(start, max_items);
    }

private:
    conn::ConnectionPool pool_;
    std::shared_ptr<PlayerRepositoryImpl> players_;
};

}  // namespace postgres