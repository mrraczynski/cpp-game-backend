#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <random>
#include <sstream>

#include "tagged.h"

namespace model {

using Dimension = int;
using Coord = Dimension;

struct Point {
    Coord x, y;
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

    void AddRoad(const Road& road) {
        roads_.emplace_back(road);
    }

    void AddBuilding(const Building& building) {
        buildings_.emplace_back(building);
    }

    void AddOffice(Office office);

private:
    using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

    Id id_;
    std::string name_;
    Roads roads_;
    Buildings buildings_;

    OfficeIdToIndex warehouse_id_to_index_;
    Offices offices_;
};

class Dog {
public:
    using Id = util::Tagged<int, Dog>;

    Dog(Point position) noexcept
        : position_{ position } {
    }

    Point GetPosition() const noexcept {
        return position_;
    }

private:
    Point position_;
};

class GameSession {
public:
    using Id = util::Tagged<std::string, GameSession>;

    GameSession(Id id, Map& map) noexcept
        : map_{ map }
        , id_(std::move(id)) {
    }

    void AddDog(Dog map) {
        dogs_.emplace_back(std::move(map));
    }

    Map& GetMap() const noexcept {
        return map_;
    }

    const Id& GetId() const noexcept {
        return id_;
    }

    const std::vector<Dog>& GetDogs() const noexcept {
        return dogs_;
    }

private:
    std::vector<Dog> dogs_;
    Map& map_;
    Id id_;
};

class Player {
public:

    Player(int id, const std::string& name, const GameSession* session) noexcept
        : id_{ id }
        , name_{ name }
        , session_{ session } { }

    const GameSession* GetSession()
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

private:
    const GameSession* session_;
    std::string name_;
    int id_;
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

    Player* FindPlayerByToken(const Token& token)
    {
        if (players_.find(*token) != players_.end())
        {
            return &(players_.at(*token));
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

    using GameSessions = std::vector<GameSession>;

    int GetPlayerId()
    {
        return ++player_id_;
    }

    Map& AddMap(Map map);

    void AddGameSession(GameSession session);

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

    const GameSession* FindGameSessionByMap(const Map::Id& id) const noexcept {
        /*for (auto& session : sessions_) {
            if (session.GetMap().GetId() == id)
            {
                return &session;
            }
        }*/
        for (auto it = sessions_.begin(); it != sessions_.end(); ++it)
        {            
            if (it->GetMap().GetId() == id)
            {
                return &(*it);
            }
        }
        return nullptr;
    }

private:
    using MapIdHasher = util::TaggedHasher<Map::Id>;
    using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;
    using GameSessionIdHasher = util::TaggedHasher<GameSession::Id>;
    using GameSessionIdToIndex = std::unordered_map<GameSession::Id, size_t, GameSessionIdHasher>;

    std::vector<Map> maps_;
    MapIdToIndex map_id_to_index_;
    std::vector<GameSession> sessions_;
    GameSessionIdToIndex session_id_to_index_;
    int player_id_ = 0;
};

}  // namespace model
