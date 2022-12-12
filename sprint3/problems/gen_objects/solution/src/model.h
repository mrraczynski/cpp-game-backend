#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include <deque>
#include <random>
#include <sstream>
#include <optional>

#include "tagged.h"
#include "loot_generator.h"

/*enum class Direction {
    NORTH,
    SOUTH,
    WEST,
    EAST
};

std::unordered_map<Direction, char> Directions{
    { Direction::NORTH, 'N' },
    { Direction::SOUTH, 'S' },
    { Direction::WEST, 'W' },
    { Direction::EAST, 'E' }
};*/

namespace model {

using namespace std::literals;
using Dimension = int;
using Coord = Dimension;
using TimeInterval = std::chrono::milliseconds;

constexpr std::string_view Directions[5] = { "U", "L", "D", "R", "" };

struct Point {
    Coord x, y;
};

struct Vector2 {
    double x, y;
};

struct Size {
    Dimension width, height;
};

struct Rectangle {
    Point position;
    Size size;
};

struct Offset {
    Dimension dx, dy;
};

struct LootType {
    std::string name;
    std::string file;
    std::string type;
    int rotation;
    std::string color;
    double scale;
    int type_num = 0;
};

struct LootObject {
    int id;
    LootType loot_type;
    Vector2 position;
};

class Road {
    struct HorizontalTag {
        explicit HorizontalTag() = default;
    };

    struct VerticalTag {
        explicit VerticalTag() = default;
    };

public:
    constexpr static HorizontalTag HORIZONTAL{};
    constexpr static VerticalTag VERTICAL{};

    Road(HorizontalTag, Point start, Coord end_x) noexcept
        : start_{start}
        , end_{end_x, start.y} {
    }

    Road(VerticalTag, Point start, Coord end_y) noexcept
        : start_{start}
        , end_{start.x, end_y} {
    }

    bool IsHorizontal() const noexcept {
        return start_.y == end_.y;
    }

    bool IsVertical() const noexcept {
        return start_.x == end_.x;
    }

    Point GetStart() const noexcept {
        return start_;
    }

    Point GetEnd() const noexcept {
        return end_;
    }

    bool IsCoordinatesOnRoad(Vector2 coord) const
    {
        return (coord.x >= (start_.x - 0.4) && coord.x <= (end_.x + 0.4))
            && (coord.y >= (start_.y - 0.4) && coord.y <= (end_.y + 0.4));
    }

private:
    Point start_;
    Point end_;
};

class Building {
public:
    explicit Building(Rectangle bounds) noexcept
        : bounds_{bounds} {
    }

    const Rectangle& GetBounds() const noexcept {
        return bounds_;
    }

private:
    Rectangle bounds_;
};

class Office {
public:
    using Id = util::Tagged<std::string, Office>;

    Office(Id id, Point position, Offset offset) noexcept
        : id_{std::move(id)}
        , position_{position}
        , offset_{offset} {
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    Point GetPosition() const noexcept {
        return position_;
    }

    Offset GetOffset() const noexcept {
        return offset_;
    }

private:
    Id id_;
    Point position_;
    Offset offset_;
};

class Map {
public:
    using Id = util::Tagged<std::string, Map>;
    using Roads = std::vector<Road>;
    using Buildings = std::vector<Building>;
    using Offices = std::vector<Office>;

    Map(Id id, std::string name) noexcept
        : id_(std::move(id))
        , name_(std::move(name)) {
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    const std::string& GetName() const noexcept {
        return name_;
    }

    const Buildings& GetBuildings() const noexcept {
        return buildings_;
    }

    const Roads& GetRoads() const noexcept {
        return roads_;
    }

    const Offices& GetOffices() const noexcept {
        return offices_;
    }

    int GetCurrentLootCount() const noexcept
    {
        return cur_loot_count_;
    }

    void AddRoad(const Road& road) {
        roads_.emplace_back(road);
    }

    void AddBuilding(const Building& building) {
        buildings_.emplace_back(building);
    }

    void AddLootType(const LootType& loot_type)
    {
        loot_types_.push_back(loot_type);
        loot_types_count_++;
        loot_types_[loot_types_.size() - 1].type_num = loot_types_count_;
    }

    void SetRandomizeSpawnPoints(bool randomize_spawns)
    {
        randomize_spawn_points_ = randomize_spawns;
    }

    void SetDogSpeed(std::optional<double> speed)
    {
        if (speed != std::nullopt)
        {
            dog_speed_ = speed;
        }
    }

    std::optional<double> GetDogSpeed() const
    {
        return dog_speed_;
    }

    void AddOffice(Office office);

    std::optional<Road> GetRandomRoad() const noexcept
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

    std::optional<Vector2> GetRandomPointOnRoad() const
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

    bool IsCoordinatesOnRoads(Vector2 coord) const
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

    void CreateLootObjects(int count) const
    {
        for (int i = 0; i < count; ++i)
        {
            auto rand_loot_type = GetRandomLootType();
            auto rand_point = GetRandomPointOnRoad();
            if (rand_loot_type && rand_point)
            {
                cur_loot_count_++;
                LootObject loot_obj{ cur_loot_count_, rand_loot_type.value(), rand_point.value()};
                loot_objects_.push_back(loot_obj);                
            }
        }
    }

    const std::vector<LootObject>& GetLootObjects() const
    {
        return loot_objects_;
    }

    const std::vector<LootType>& GetLootTypes() const
    {
        return loot_types_;
    }

private:

    std::optional<LootType> GetRandomLootType() const
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

    using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

    Id id_;
    std::string name_;
    Roads roads_;
    Buildings buildings_;

    OfficeIdToIndex warehouse_id_to_index_;
    Offices offices_;
    
    std::optional<double> dog_speed_ = std::nullopt;
    bool randomize_spawn_points_ = false;

    std::vector<LootType> loot_types_;
    int loot_types_count_ = 0;
    mutable std::vector<LootObject> loot_objects_;
    mutable int cur_loot_count_ = 0;
};

class Dog {
public:

    Dog() noexcept
        : position_{ Vector2({0,0}) }
        , dir_{ 0 }
        , speed_{ Vector2({0,0}) } {
        //position_ = session_.GetMap().GetRandomPointOnRoad();
    }

    Vector2 GetPosition() const noexcept {
        return position_;
    }

    Vector2 GetSpeed() const noexcept {
        return speed_;
    }

    int GetDirection() noexcept
    {
        return dir_;
    }

    void SetDirection(int dir) noexcept
    {
        dir_ = dir;
    }

    void SetSpeed(Vector2 speed) noexcept
    {
        speed_ = speed;
    }

    void SetPosition(Vector2 pos) noexcept
    {
        position_ = pos;
    }

private:
    Vector2 position_;
    Vector2 speed_;
    int dir_;
};

class GameSession {
public:
    using Id = util::Tagged<std::string, GameSession>;

    GameSession(Id id, Map::Id map_id, std::optional<double> dog_speed) noexcept
        : map_id_{ map_id }
        , id_(std::move(id))
        , dog_speed_{ dog_speed } {
    }

    Map::Id GetMapId() const noexcept {
        return map_id_;
    }

    GameSession* GetThisGameSession()
    {
        return this;
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    const std::optional<double>& GetDogSpeed() const
    {
        return dog_speed_;
    }

private:
    Map::Id map_id_;
    Id id_;
    std::optional<double> dog_speed_;
};

class Player {
public:

    Player(int id, const std::string& name, Vector2 dog_pos, const GameSession* session) noexcept
        : id_{ id }
        , name_{ name }
        , session_{ session }
    { 
        dog_.SetPosition(dog_pos);
    }

    const GameSession* GetSession() const
    {
        return session_;
    }

    const std::string& GetName()
    {
        return name_;
    }

    int GetId()
    {
        return id_;
    }

    void SetDogDirection(const std::string_view& dir, double speed) const;

    Dog& GetDog()
    {
        return dog_;
    }

    template<typename Func>
    void TickPlayer(double delta_time, double game_dog_speed, Func&& map_searcher)
    {
        const std::optional<double>& session_dog_speed = session_->GetDogSpeed();
        const Map* map = map_searcher(session_->GetMapId());
        if (session_dog_speed != std::nullopt)
        {
            double speed_x = session_dog_speed.value() * delta_time;
            double speed_y = session_dog_speed.value() * delta_time;
            Vector2 new_pos{
                dog_.GetPosition().x + session_dog_speed.value() * delta_time,
                dog_.GetPosition().y + session_dog_speed.value() * delta_time
            };
            if (map->IsCoordinatesOnRoads(new_pos))
            {
                dog_.SetPosition(new_pos);
                dog_.SetSpeed(Vector2{ speed_x, speed_y });
            }
            else
            {
                SetDogDirection(""sv, 0);
                dog_.SetSpeed(Vector2{ 0, 0 });
            }
        }
        else
        {
            double speed_x = game_dog_speed * delta_time;
            double speed_y = game_dog_speed * delta_time;
            Vector2 new_pos{
                dog_.GetPosition().x * game_dog_speed * delta_time,
                dog_.GetPosition().y * game_dog_speed * delta_time
            };
            if (map->IsCoordinatesOnRoads(new_pos))
            {
                dog_.SetPosition(new_pos);
                dog_.SetSpeed(Vector2{ speed_x, speed_y });
            }
            else
            {
                SetDogDirection(""sv, 0);
                dog_.SetSpeed(Vector2{ 0, 0 });
            }
        }
    }

private:
    const GameSession* session_;
    std::string name_;
    int id_;
    mutable Dog dog_;
};

namespace detail {
    struct TokenTag {};
}  // namespace detail

using Token = util::Tagged<std::string, detail::TokenTag>;

class PlayerTokens {
public:

    Token AddPlayer(Player& player)
    {
        Token token = GenerateToken();
        players_.insert({ *token, player });
        return token;
    }

    const Player* FindPlayerByToken(const Token& token)
    {
        if (players_.find(*token) != players_.end())
        {
            return &players_.at(*token);
        }     
        return nullptr;
    }

    std::vector<Player> GetPlayersBySession(const GameSession* session)
    {
        std::vector<Player> session_players;
        for (auto& player : players_)
        {
            if (player.second.GetSession()->GetId() == session->GetId())
            {
                session_players.push_back(player.second);
            }
        }
        return session_players;
    }

    static PlayerTokens& GetInstance()
    {
        static PlayerTokens instance;
        return instance;
    }

    template<typename Func>
    void TickPlayers(double delta_time, double game_dog_speed, Func&& map_searcher)
    {
        for (auto& player : players_)
        {
            player.second.TickPlayer(delta_time, game_dog_speed, map_searcher);
        }
    }
    
private:
    std::random_device random_device_;
    std::mt19937_64 generator1_{ [this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }() };
    std::mt19937_64 generator2_{ [this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }() };
    // ����� ������������� �����, �������� �� generator1_ � generator2_
    // ��� 64-��������� ����� �, �������� �� � hex-������, ������� � ����.
    // �� ������ �������������������� � ���������� ������������� �������,
    // ����� ������� �� ������ ��� ����� ���������������

    Token GenerateToken()
    {
        std::ostringstream ss;
        ss << std::hex << generator1_() << generator2_();
        std::string ss_str = ss.str();
        ss_str.resize(32, '0');
        Token token(ss_str);
        return token;
    }

    std::unordered_map<std::string, Player> players_;
};

class Game {
public:
    using Maps = std::vector<Map>;

    using GameSessions = std::deque<GameSession>;

    int GetPlayerId()
    {
        return ++player_id_;
    }

    void AddMap(Map map);

    void AddGameSession(GameSession session);

    void SetDogSpeed(double speed)
    {
        default_dog_speed_ = speed;
    }

    void SetLootPeriod(double loot_period)
    {
        loot_period_ = loot_period;
    }

    void SetLootProbability(double loot_probability)
    {
        loot_probability_ = loot_probability;
    }

    double GetLootPeriod()
    {
        return loot_period_;
    }

    double GetLootProbability()
    {
        return loot_probability_;
    }

    const Maps& GetMaps() const noexcept {
        return maps_;
    }

    const GameSessions& GetGameSessions() const noexcept {
        return sessions_;
    }

    const Map* FindMap(const Map::Id& id) const noexcept {
        if (auto it = map_id_to_index_.find(id); it != map_id_to_index_.end()) {
            return &maps_.at(it->second);
        }
        return nullptr;
    }

    const GameSession* FindGameSession(const GameSession::Id& id) const noexcept {
        if (auto it = session_id_to_index_.find(id); it != session_id_to_index_.end()) {
            return &sessions_.at(it->second);
        }
        return nullptr;
    }

    void SetPlayerDir(const std::string_view& token, const std::string_view& dir);

    const GameSession* FindGameSessionByMap(Map::Id id) noexcept;    

    std::string GetCurrentGameState(const std::string_view& token, std::string& error_code);

    void TickGame(double delta_time_msec)
    {
        double delta_time_sec = delta_time_msec / 1000;
        PlayerTokens::GetInstance().TickPlayers(delta_time_sec, default_dog_speed_,
            [&](const Map::Id& id) {
            return FindMap(id);
        });
    }

    void GenerateLoot(TimeInterval time_delta)
    {
        for (auto& session : sessions_)
        {
            int looter_count = PlayerTokens::GetInstance().GetPlayersBySession(&session).size();
            auto map = FindMap(session.GetMapId());
            if (map != nullptr)
            {
                int loot_count = map->GetCurrentLootCount();
                unsigned count = loot_generator_.Generate(time_delta, loot_count, looter_count);
                map->CreateLootObjects(count);
            }
        }
    }

private:
    using MapIdHasher = util::TaggedHasher<Map::Id>;
    using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;
    using GameSessionIdHasher = util::TaggedHasher<GameSession::Id>;
    using GameSessionIdToIndex = std::unordered_map<GameSession::Id, size_t, GameSessionIdHasher>;

    std::vector<Map> maps_;
    MapIdToIndex map_id_to_index_;

    std::deque<GameSession> sessions_;
    GameSessionIdToIndex session_id_to_index_;

    int player_id_ = 0;
    double default_dog_speed_ = 1;

    double loot_period_ = 5.0f;
    double loot_probability_ = .5f;
    loot_gen::LootGenerator loot_generator_{ TimeInterval(10ms), loot_probability_};
};

}  // namespace model