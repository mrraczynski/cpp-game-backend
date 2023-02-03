#pragma once
#include "postgres.h"

#include <pqxx/zview.hxx>
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>

namespace postgres {

using namespace std::literals;
using pqxx::operator"" _zv;
namespace uuid = boost::uuids;

void PlayerRepositoryImpl::SaveProgress(const model::Player& player)
{
    Wrapper conn = pool_.GetConnection();
    pqxx::work work(*conn);
    std::string uuid = uuid::to_string(uuid::random_generator()());

    work.exec_params(R"(
INSERT INTO hall_of_fame (id, name, score, play_time) VALUES ($1, $2, $3, $4);
)"_zv, uuid, player.GetName(), player.GetScore(), (int)player.GetPlayTime());

    work.commit();
}

std::vector<model::PlayerRepositoryAttributes> PlayerRepositoryImpl::GetProgress(int start, int max_items) const
{
    try {
        Wrapper conn = pool_.GetConnection();
        pqxx::read_transaction read(*conn);

        auto res = read.exec_params(R"(
SELECT name, score, play_time FROM hall_of_fame ORDER BY score DESC, play_time ASC, name ASC LIMIT $1 OFFSET $2;
)"_zv, max_items, start);

        std::vector<model::PlayerRepositoryAttributes> rows;

        for (const auto& row : res)
        {
            rows.push_back(model::PlayerRepositoryAttributes{ row[0].c_str(), row[1].get<int>().value(), row[2].get<int>().value() });
        }
        return rows;
    }
    catch (std::exception& e)
    {
        std::cout << e.what();
    }
}

Database::Database(const unsigned pool_size, std::string db_url)
    : pool_(pool_size, std::move(db_url))
    , players_(std::make_shared<PlayerRepositoryImpl>(pool_)) {

    Wrapper conn = pool_.GetConnection();
    pqxx::work work(*conn);

    players_->SetSignals();

    work.exec(R"(
CREATE TABLE IF NOT EXISTS hall_of_fame (
    id UUID CONSTRAINT hall_of_fame_id_constraint PRIMARY KEY,
    name varchar(100) NOT NULL,
    score integer NOT NULL,
    play_time integer NOT NULL
);
)"_zv);
    work.exec(R"(
CREATE INDEX IF NOT EXISTS hall_of_fame_id_idx ON hall_of_fame (id); 
)"_zv);

    // коммитим изменения
    work.commit();
}

}  // namespace postgres