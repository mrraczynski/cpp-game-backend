#include "model.h"
#include "json_loader.h"

#include <stdexcept>

namespace model {
using namespace std::literals;

const GameSession* Game::FindGameSessionByMap(Map::Id id) noexcept {
    for (auto& session : sessions_)
    {
        if (session.GetMapId() == id)
        {
            if (auto it = session_id_to_index_.find(session.GetId()); it != session_id_to_index_.end())
            {
                return &sessions_.at(it->second);
            }
        }
    }
    return nullptr;
}

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

std::optional<Vector2> Map::GetRandomPointOnRoad() const
{
    if (!randomize_spawn_points_)
    {
        if (roads_.size() > 0)
        {
            double tmp_x = roads_[0].GetStart().x;
            double tmp_y = roads_[0].GetStart().y;
            return Vector2{ tmp_x, tmp_y };
        }
        else
        {
            return std::nullopt;
        }
    }
    else
    {
        auto road = GetRandomRoad();
        if (road)
        {
            std::random_device dev;
            std::uniform_real_distribution<double> dist(0, 1);
            double rand = dist(dev);

            auto dist_x = road.value().GetEnd().x - road.value().GetStart().x;
            auto dist_y = road.value().GetEnd().y - road.value().GetStart().y;

            auto mod_x = (dist_x * rand) + road.value().GetStart().x;
            auto mod_y = (dist_y * rand) + road.value().GetStart().y;

            double tmp_x = road.value().GetStart().x;
            double tmp_y = road.value().GetStart().y;
            return Vector2({ mod_x, mod_y });
        }
        else
        {
            return std::nullopt;
        }
    }
}

bool Map::IsCoordinatesOnRoads(Vector2 coord) const
{
    int counter = 0;
    for (auto& road : roads_)
    {
        if (road.IsCoordinatesOnRoad(coord))
        {
            counter++;
        }
    }
    if (counter > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::optional<Road> Map::GetRandomRoad() const noexcept
{
    if (roads_.size() > 0)
    {
        std::random_device dev;
        std::uniform_int_distribution<int> dist(1, roads_.size());
        //return roads_[dist(dev) - 1];
        return roads_[0];
    }
    else
    {
        return std::nullopt;
    }
}

std::optional<LootType> Map::GetRandomLootType() const
{
    if (loot_types_.size() > 0)
    {
        std::random_device dev;
        std::uniform_int_distribution<int> dist(1, loot_types_.size());
        return loot_types_[dist(dev) - 1];
    }
    else
    {
        return std::nullopt;
    }
}

void Map::CreateLootObjects(int count) const
{
    for (int i = 0; i < count; ++i)
    {
        auto rand_loot_type = GetRandomLootType();
        auto rand_point = GetRandomPointOnRoad();
        if (rand_loot_type && rand_point)
        {
            cur_loot_count_++;
            LootObject loot_obj{ cur_loot_count_, rand_loot_type.value(), rand_point.value() };
            loot_objects_.push_back(loot_obj);
        }
    }
}

void Game::AddMap(Map map) {
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
    const model::Player* player = model::PlayerTokens::GetInstance().FindPlayerByToken(model::Token(std::string(token)));
    if (player == nullptr)
    {
        std::string body_str;
        error_code = "unknownToken";
        return body_str;
    }
    std::string body_str;    
    auto session = FindGameSession(player->GetSessionID());
    auto map = FindMap(session->GetMapId());
    std::vector<model::Player> session_players = model::PlayerTokens::GetInstance().GetPlayersBySession(player->GetSessionID());
    return json_loader::CreatePlayersWithParametersArray(session_players, map);
}

void Player::SetDogDirection(const std::string_view& dir, double speed) const
{
    if (dir == "U"sv)
    {
        dog_.SetDirection(0);
        dog_.SetSpeed(Vector2{ 0, -speed });
    }
    else if (dir == "L"sv)
    {
        dog_.SetDirection(1);
        dog_.SetSpeed(Vector2{ -speed, 0 });
    }
    else if (dir == "D"sv)
    {
        dog_.SetDirection(2);
        dog_.SetSpeed(Vector2{ 0, speed });
    }
    else if (dir == "R"sv)
    {
        dog_.SetDirection(3);
        dog_.SetSpeed(Vector2{ speed, 0 });
    }
    else if (dir.size() == 0)
    {
        dog_.SetDirection(4);
        dog_.SetSpeed(Vector2{ 0, 0 });
    }
}

void Player::TickPlayer(double delta_time, double game_dog_speed, std::function<const Map* (const Map::Id&)> map_searcher, std::function<const GameSession* (const GameSession::Id&)> session_searcher)
{
    auto session = session_searcher(session_id_);
    const std::optional<double>& session_dog_speed = session->GetDogSpeed();
    const Map* map = map_searcher(session->GetMapId());
    if (session_dog_speed != std::nullopt)
    {
        double speed_x = session_dog_speed.value() * delta_time;
        double speed_y = session_dog_speed.value() * delta_time;
        Vector2 new_pos{
            dog_.GetPosition().x + dog_.GetSpeed().x * delta_time,
            dog_.GetPosition().y + dog_.GetSpeed().y * delta_time
        };
        if (map->IsCoordinatesOnRoads(new_pos))
        {
            dog_.SetPosition(new_pos);
            //dog_.SetSpeed(Vector2{ speed_x, speed_y });
        }
        else
        {
            //SetDogDirection(""sv, 0);
            dog_.SetSpeed(Vector2{ 0, 0 });
        }
    }
    else
    {
        double speed_x = game_dog_speed * delta_time;
        double speed_y = game_dog_speed * delta_time;
        Vector2 new_pos{
            dog_.GetPosition().x + dog_.GetSpeed().x * delta_time,
            dog_.GetPosition().y + dog_.GetSpeed().y * delta_time
        };
        if (map->IsCoordinatesOnRoads(new_pos))
        {
            dog_.SetPosition(new_pos);
            //dog_.SetSpeed(Vector2{ speed_x, speed_y });
        }
        else
        {
            //SetDogDirection(""sv, 0);
            dog_.SetSpeed(Vector2{ 0, 0 });
        }
    }
}

void Game::SetPlayerDir(const std::string_view& token, const std::string_view& dir)
{
    int player_ec;
    const model::Player* player = model::PlayerTokens::GetInstance().FindPlayerByToken(model::Token(std::string(token)));
    std::optional<double> def_speed = FindMap(FindGameSession(player->GetSessionID())->GetMapId())->GetDogSpeed();
    if (def_speed != std::nullopt)
    {
        player->SetDogDirection(dir, def_speed.value());
    }
    else
    {
        player->SetDogDirection(dir, default_dog_speed_);
    }
}

void Game::TickGame(double time_delta)
{
    double delta_time_sec = time_delta / 1000;
    PlayerTokens::GetInstance().TickPlayers(delta_time_sec, default_dog_speed_,
        [&](const Map::Id& id) {
            return FindMap(id);
        },
        [&](const GameSession::Id& id) {
            return FindGameSession(id);
        });
    CheckCollisions();
    GenerateLoot(TimeInterval((int)time_delta));
}

void Game::GenerateLoot(TimeInterval time_delta)
{
    for (auto& session : sessions_)
    {
        int looter_count = PlayerTokens::GetInstance().GetPlayersBySession(session.GetId()).size();
        auto map = FindMap(session.GetMapId());
        if (map != nullptr)
        {
            int loot_count = map->GetCurrentLootCount();
            unsigned count = loot_generator_.Generate(time_delta, loot_count, looter_count);
            map->CreateLootObjects(count);
        }
    }
}

void Game::CheckCollisions()
{
    for (auto& session : sessions_)
    {
        auto players = PlayerTokens::GetInstance().GetPlayersBySession(session.GetId());
        std::vector<Gatherer> gatherers;
        for (size_t it = 0; it < players.size(); it++)
        {
            auto dog_start_position = players[it].GetDog().GetPrevPosition();
            auto dog_end_position = players[it].GetDog().GetPosition();
            Gatherer gatherer{
                .start_pos = geom::Point2D {dog_start_position.x, dog_start_position.y},
                .end_pos = geom::Point2D {dog_end_position.x, dog_end_position.y},
                .width = 0.6,
                .gatherer_id = it
            };
            gatherers.push_back(gatherer);
        }
        auto map = FindMap(session.GetMapId());
        auto& loot_objects = map->GetLootObjects();
        std::vector<Item> items;
        for (size_t it = 0; it < loot_objects.size(); it++)
        {
            auto loot_position = loot_objects[it].position;
            Item item{
                .position = geom::Point2D {loot_position.x, loot_position.y},
                .width = 0,
                .item_id = it
            };
            items.push_back(item);
        }

        auto& offices_vec = map->GetOffices();
        std::vector<Item> offices;

        for (size_t it = 0; it < offices_vec.size(); it++)
        {
            auto office_position = offices_vec[it].GetPosition();
            Item office{
                .position = geom::Point2D {(double)office_position.x, (double)office_position.y},
                .width = 0.5,
                .item_id = it
            };
            offices.push_back(office);
        }

        ItemGathererProvider provider{ items, gatherers };
        auto events = FindGatherEvents(provider);

        if (events.size() > 0)
        {
            std::unordered_map<size_t, GatheringEvent> items_gatherers;
            for (auto& event : events)
            {
                if (auto ig = items_gatherers.find(event.item_id); ig != items_gatherers.end())
                {
                    if (event.time < ig->second.time)
                    {
                        auto map_bag_capacity = map->GetBagCapacity();
                        int bag_capacity = map_bag_capacity != std::nullopt ? map_bag_capacity.value() : default_bag_capacity_;
                        if (bag_capacity > players[event.gatherer_id].GetCurrentItemsCount())
                        {
                            items_gatherers[event.item_id] = event;
                        }
                    }
                }
                else
                {
                    items_gatherers[event.item_id] = event;
                }
            }

            for (auto& ig : items_gatherers)
            {
                LootObject loot_obj = map->CollectLootObject(ig.first);
                PlayerTokens::GetInstance().AddItemToPlayer(loot_obj, players[ig.second.gatherer_id].GetId());
            }
        }

        ItemGathererProvider office_provider{ offices, gatherers };
        auto office_events = FindGatherEvents(office_provider);

        if (office_events.size() > 0)
        {
            for (auto& ig : office_events)
            {
                PlayerTokens::GetInstance().CollectPlayerLoot(players[ig.gatherer_id].GetId());
            }
        }
    }
}

std::vector<Player> PlayerTokens::GetPlayersBySession(const GameSession::Id& session_id)
{
    std::vector<Player> session_players;
    for (auto& player : players_)
    {
        if (player.second.GetSessionID() == session_id)
        {
            session_players.push_back(player.second);
        }
    }
    return session_players;
}

const Player* PlayerTokens::FindPlayerByToken(const Token& token)
{
    if (players_.find(*token) != players_.end())
    {
        return &players_.at(*token);
    }
    return nullptr;
}

void PlayerTokens::TickPlayers(double delta_time, 
    double game_dog_speed, 
    std::function<const Map* (const Map::Id&)> map_searcher, 
    std::function<const GameSession* (const GameSession::Id&)> session_searcher)
{
    for (auto& player : players_)
    {
        player.second.TickPlayer(delta_time, game_dog_speed, map_searcher, session_searcher);
    }
}

}  // namespace model
