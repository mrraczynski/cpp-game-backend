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
#include "collision_detector.h"

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
using namespace collision_detector;
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
    int value = 0;
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

    void AddLootType(const LootType& loot_type, const std::string& raw_loot_type)
    {
        loot_types_.push_back(loot_type);
        loot_types_[loot_types_.size() - 1].type_num = loot_types_count_;
        loot_types_count_++;
        raw_loot_types_.push_back(raw_loot_type);
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

    void SetBagCapacity(std::optional<double> capacity)
    {
        if (capacity != std::nullopt)
        {
            bag_capacity_ = capacity;
        }
    }

    std::optional<double> GetDogSpeed() const
    {
        return dog_speed_;
    }

    std::optional<double> GetBagCapacity() const
    {
        return dog_speed_;
    }

    void AddOffice(Office office);

    std::optional<Road> GetRandomRoad() const noexcept;

    std::optional<Vector2> GetRandomPointOnRoad() const;

    bool IsCoordinatesOnRoads(Vector2 coord) const;

    void CreateLootObjects(int count) const;

    const std::vector<LootObject>& GetLootObjects() const
    {
        return loot_objects_;
    }

    LootObject CollectLootObject(int i) const
    {
        LootObject loot_obj = loot_objects_[i];
        loot_objects_.erase(loot_objects_.begin() + i);
        return loot_obj;
    }

    const std::vector<LootType>& GetLootTypes() const
    {
        return loot_types_;
    }

    const std::vector<std::string>& GetLootTypesRaw() const
    {
        return raw_loot_types_;
    }

private:

    std::optional<LootType> GetRandomLootType() const;

    using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

    Id id_;
    std::string name_;
    Roads roads_;
    Buildings buildings_;

    OfficeIdToIndex warehouse_id_to_index_;
    Offices offices_;
    
    std::optional<double> dog_speed_ = std::nullopt;
    std::optional<double> bag_capacity_ = std::nullopt;
    bool randomize_spawn_points_ = false;

    std::vector<LootType> loot_types_;
    int loot_types_count_ = 0;
    mutable std::vector<LootObject> loot_objects_;
    mutable int cur_loot_count_ = 0;
    std::vector<std::string> raw_loot_types_;
};

class Dog {
public:

    Dog() noexcept
        : position_{ Vector2({0,0}) }
        , dir_{ 0 }
        , speed_{ Vector2({0,0}) }
        , prev_position_{ Vector2({0,0}) }
    {
        //position_ = session_.GetMap().GetRandomPointOnRoad();
    }

    Vector2 GetPosition() const noexcept {
        return position_;
    }

    Vector2 GetPrevPosition() const noexcept {
        return prev_position_;
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
        prev_position_ = position_;
        position_ = pos;
    }

private:
    Vector2 position_;
    Vector2 prev_position_;
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

    const std::vector<LootObject> GetCurrentBag()
    {
        return bag_;
    }

    void AddItemToBag(LootObject obj)
    {
        bag_.push_back(obj);
        cur_bag_++;
    }

    int GetCurrentItemsCount()
    {
        return cur_bag_;
    }

    void CollectPoints()
    {
        for (auto& obj : bag_)
        {
            score_ = score_ + obj.loot_type.value;
        }
        bag_ = std::vector<LootObject>();
        cur_bag_ = 0;
    }

    int GetScore()
    {
        return score_;
    }

    void SetDogDirection(const std::string_view& dir, double speed) const;

    Dog& GetDog()
    {
        return dog_;
    }

    void TickPlayer(double delta_time, double game_dog_speed, std::function<const Map* (const Map::Id&)> map_searcher);

private:
    const GameSession* session_;
    std::string name_;
    int id_;
    mutable Dog dog_;
    std::vector<LootObject> bag_;
    int cur_bag_ = 0;
    int score_ = 0;
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

    void AddItemToPlayer(LootObject obj, int id)
    {
        GetPlayer(id).AddItemToBag(obj);
    }

    void CollectPlayerLoot(int id)
    {
        GetPlayer(id).CollectPoints();
    }

    const Player* FindPlayerByToken(const Token& token);

    std::vector<Player> GetPlayersBySession(const GameSession* session);

    static PlayerTokens& GetInstance()
    {
        static PlayerTokens instance;
        return instance;
    }

    void TickPlayers(double delta_time, double game_dog_speed, std::function<const Map* (const Map::Id&)> map_searcher);
    
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
    // Чтобы сгенерировать токен, получите из generator1_ и generator2_
    // два 64-разрядных числа и, переведя их в hex-строки, склейте в одну.
    // Вы можете поэкспериментировать с алгоритмом генерирования токенов,
    // чтобы сделать их подбор ещё более затруднительным

    Player& GetPlayer(int id)
    {
        for (auto& player : players_)
        {
            if (player.second.GetId() == id)
            {
                return player.second;
            }
        }
    }

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

    void SetDefBagCapacity(double default_bag_capacity)
    {
        default_bag_capacity_ = default_bag_capacity;
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

    void TickGame(double time_delta);

    void GenerateLoot(TimeInterval time_delta);

private:

    void CheckCollisions();

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
    double default_bag_capacity_ = 3;

    double loot_period_ = 5.0f;
    double loot_probability_ = .5f;
    loot_gen::LootGenerator loot_generator_{ TimeInterval(10ms), loot_probability_};
};

}  // namespace model
