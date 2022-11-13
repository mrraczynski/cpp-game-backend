#include "model.h"
#include "json_loader.h"

#include <stdexcept>

namespace model {
using namespace std::literals;

void Map::AddOffice(Office office) {
    if (warehouse_id_to_index_.contains(office.GetId())) {
        throw std::invalid_argument("Duplicate warehouse");
    }

    const size_t index = offices_.size();
    Office& o = offices_.emplace_back(std::move(office));
    try {
        warehouse_id_to_index_.emplace(o.GetId(), index);
    } catch (...) {
        // Удаляем офис из вектора, если не удалось вставить в unordered_map
        offices_.pop_back();
        throw;
    }
}

Game::AddMap(Map map) {
    const size_t index = maps_.size();
    if (auto [it, inserted] = map_id_to_index_.emplace(map.GetId(), index); !inserted) {
        throw std::invalid_argument("Map with id "s + *map.GetId() + " already exists"s);
    } else {
        try {
            maps_.emplace_back(std::move(map));
        } catch (...) {
            map_id_to_index_.erase(it);
            throw;
        }
    }
}

void Game::AddGameSession(GameSession session) {
    const size_t index = sessions_.size();
    if (auto [it, inserted] = session_id_to_index_.emplace(session.GetId(), index); !inserted) {
        throw std::invalid_argument("Session with id "s + *session.GetId() + " already exists"s);
    }
    else {
        try {
            sessions_.emplace_back(std::move(session));
        }
        catch (...) {
            session_id_to_index_.erase(it);
            throw;
        }
    }
}

std::string Game::GetCurrentGameState(const std::string_view& token, std::string& error_code)
{
    int player_ec;
    const model::Player* player = model::PlayerTokens::GetInstance().FindPlayerByToken(model::Token(std::string(token)), player_ec);
    if (player_ec != 0)
    {
        std::string body_str;
        error_code = "unknownToken";
        return body_str;
    }
    std::string body_str;
    std::vector<model::Player> session_players = model::PlayerTokens::GetInstance().GetPlayersBySession(player->GetSession());
    return json_loader::CreatePlayersWithParametersArray(session_players);
}

}  // namespace model
