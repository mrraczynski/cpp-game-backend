#include <boost/serialization/vector.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/unordered_map.hpp>

#include "model.h"

namespace geom {

template <typename Archive>
void serialize(Archive& ar, Point2D& point, [[maybe_unused]] const unsigned version) {
    ar& point.x;
    ar& point.y;
}

}  // namespace geom

namespace model {

template <typename Archive>
void serialize(Archive& ar, Vector2& vec, [[maybe_unused]] const unsigned version) {
    ar& vec.x;
    ar& vec.y;
}

template <typename Archive>
void serialize(Archive& ar, Point& p, [[maybe_unused]] const unsigned version) {
    ar& p.x;
    ar& p.y;
}

template <typename Archive>
void serialize(Archive& ar, Size& size, [[maybe_unused]] const unsigned version) {
    ar& size.width;
    ar& size.height;
}

template <typename Archive>
void serialize(Archive& ar, Rectangle& rect, [[maybe_unused]] const unsigned version) {
    ar& rect.position;
    ar& rect.size;
}

template <typename Archive>
void serialize(Archive& ar, Offset& ofs, [[maybe_unused]] const unsigned version) {
    ar& ofs.dx;
    ar& ofs.dy;
}

template <typename Archive>
void serialize(Archive& ar, LootType& type, [[maybe_unused]] const unsigned version) {
    ar& (type.name);
    ar& (type.file);
    ar& (type.type);
    ar& (type.rotation);
    ar& (type.color);
    ar& (type.scale);
    ar& (type.type_num);
    ar& (type.value);
}

template <typename Archive>
void serialize(Archive& ar, LootObject& obj, [[maybe_unused]] const unsigned version) {
    ar&(obj.id);
    ar&(obj.loot_type);
    ar& (obj.position);
}

}  // namespace model

namespace serialization {

    class RoadRepr {
    public:
        RoadRepr() = default;

        explicit RoadRepr(const model::Road& road)
            : start_(road.GetStart())
            , end_(road.GetEnd()) {
        }

        [[nodiscard]] model::Road Restore() const {
            model::Road road(start_, end_);           
            return road;
        }

        template <typename Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
            ar& start_;
            ar& end_;
        }

    private:
        model::Point start_;
        model::Point end_;
    };

    class BuildingRepr {
    public:
        BuildingRepr() = default;

        explicit BuildingRepr(const model::Building& road)
            : bounds_(road.GetBounds()) {
        }

        [[nodiscard]] model::Building Restore() const {
            model::Building building(bounds_);
            return building;
        }

        template <typename Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
            ar& bounds_;
        }

    private:
        model::Rectangle bounds_;
    };

    class OfficeRepr {
    public:
        OfficeRepr() 
            : id_(model::Office::Id(std::string()))
        {};

        explicit OfficeRepr(const model::Office& office)
            : id_(office.GetId())
            , position_(office.GetPosition())
            , offset_(office.GetOffset()) {
        }

        [[nodiscard]] model::Office Restore() const {
            model::Office office(id_, position_, offset_);
            return office;
        }

        template <typename Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
            ar& (*id_);
            ar& position_;
            ar& offset_;
        }

    private:
        model::Office::Id id_;
        model::Point position_;
        model::Offset offset_;
    };

    class GameSessionRepr {
    public:
        GameSessionRepr() 
            : map_id_ (model::Map::Id(std::string()))
            , id_(model::GameSession::Id(std::string()))
            , dog_speed_(0)
        {};

        explicit GameSessionRepr(const model::GameSession& session)
            : map_id_(session.GetMapId())
            , id_(session.GetId())
            , dog_speed_(session.GetDogSpeed() != std::nullopt ? session.GetDogSpeed().value() : 0) {
        }

        [[nodiscard]] model::GameSession Restore() const {
            model::GameSession session(id_, map_id_, dog_speed_);
            return session;
        }

        template <typename Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
            ar& (*id_);
            ar& (*map_id_);
            ar& dog_speed_;
        }

    private:
        model::Map::Id map_id_;
        model::GameSession::Id id_;
        double dog_speed_;
    };

    class MapRepr {
    public:
        MapRepr() 
            : id_(model::Map::Id(std::string()))
        {};

        explicit MapRepr(const model::Map& map)
            : id_(map.GetId())
            , name_(map.GetName())
            , dog_speed_(map.GetDogSpeed() != std::nullopt ? map.GetDogSpeed().value() : 0)
            , bag_capacity_(map.GetBagCapacity() != std::nullopt ? map.GetBagCapacity().value() : 0)
            , randomize_spawn_points_(map.IsRandomSpawnPoints())
            , loot_types_(map.GetLootTypes())
            , loot_objects_(map.GetLootObjects())
            , raw_loot_types_(map.GetLootTypesRaw()) 
        {

            for (auto& road : map.GetRoads())
            {
                roads_.push_back(RoadRepr(road));
            }

            for (auto& building : map.GetBuildings())
            {
                buildings_.push_back(BuildingRepr(building));
            }

            for (auto& office : map.GetOffices())
            {
                offices_.push_back(OfficeRepr(office));
            }

        }

        [[nodiscard]] model::Map Restore() const {
            model::Map map(id_, name_);            
            for (auto& road : roads_)
            {
                map.AddRoad(road.Restore());
            }
            for (auto& building : buildings_)
            {
                map.AddBuilding(building.Restore());
            }
            for (auto& office : offices_)
            {
                map.AddOffice(office.Restore());
            }
            map.SetDogSpeed(dog_speed_);
            map.SetBagCapacity(bag_capacity_);
            map.SetRandomizeSpawnPoints(randomize_spawn_points_);
            for (int i = 0; i < loot_types_.size(); i++)
            {
                map.AddLootType(loot_types_[i], raw_loot_types_[i]);
            }           
            return map;
        }

        template <typename Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
            ar& (*id_);
            ar& name_;
            ar& roads_;
            ar& buildings_;
            ar& offices_;
            ar& dog_speed_;
            ar& bag_capacity_;
            ar& randomize_spawn_points_;
            ar& loot_types_;
            ar& loot_objects_;
            ar& raw_loot_types_;
        }

    private:
        model::Map::Id id_;
        std::string name_;
        std::vector<RoadRepr> roads_;
        std::vector<BuildingRepr> buildings_;
        std::vector<OfficeRepr> offices_;

        double dog_speed_;
        double bag_capacity_;
        bool randomize_spawn_points_ = false;

        std::vector<model::LootType> loot_types_;
        std::vector<model::LootObject> loot_objects_;
        std::vector<std::string> raw_loot_types_;
    };

    // DogRepr (DogRepresentation) - сериализованное представление класса Dog
    class DogRepr {
    public:
        DogRepr() = default;

        explicit DogRepr(const model::Dog& dog)
            : position_(dog.GetPosition())
            , prev_position_(dog.GetPrevPosition())
            , speed_(dog.GetSpeed())
            , dir_(dog.GetDirection()) {
        }

        [[nodiscard]] model::Dog Restore() const {
            model::Dog dog(position_, dir_, speed_, prev_position_);
            return dog;
        }

        template <typename Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
            ar& position_;
            ar& prev_position_;
            ar& speed_;
            ar& dir_;
        }

    private:
        model::Vector2 position_;
        model::Vector2 prev_position_;
        model::Vector2 speed_;
        int dir_ = 0;
    };

    class PlayerRepr {
    public:
        PlayerRepr() 
            : session_id_(model::GameSession::Id(std::string())) {};

        explicit PlayerRepr(const model::Player& player)
            : session_id_(player.GetSessionID())
            , name_(player.GetName())
            , id_(player.GetId())
            , dog_(DogRepr(player.GetDog()))
            , bag_(player.GetBag())
            , cur_bag_(player.GetCurrentItemsCount())
            , score_(player.GetScore()) {
        }

        [[nodiscard]] model::Player Restore() {
            model::Player player(id_, name_, session_id_, dog_.Restore(), std::move(bag_), cur_bag_, score_);
            return player;
        }

        template <typename Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
            ar& (*session_id_);
            ar& name_;
            ar& id_;
            ar& dog_;
            ar& bag_;
            ar& cur_bag_;
            ar& score_;
        }

    private:
        model::GameSession::Id session_id_;
        std::string name_;
        int id_ = 0;
        DogRepr dog_;
        std::vector<model::LootObject> bag_;
        int cur_bag_ = 0;
        int score_ = 0;
    };

    class GameRepr {
    public:
        GameRepr() = default;

        explicit GameRepr(const model::Game& game)
            : player_id_(game.GetPlayerCurId())
            , default_dog_speed_(game.GetDefDogSpeed())
            , default_bag_capacity_(game.GetDefBagCapacity())
            , loot_period_(game.GetLootPeriod())
            , loot_probability_(game.GetLootProbability()) 
        {
            auto maps = game.GetMaps();
            for (int i = 0; i < maps.size(); i++)
            {
                maps_.push_back(MapRepr(maps[i]));
            }
            auto sessions = game.GetGameSessions();
            for (int i = 0; i < sessions.size(); i++)
            {
                sessions_.push_back(GameSessionRepr(sessions[i]));
            }
            auto& players = model::PlayerTokens::GetInstance().GetPlayers();
            for (auto& player : players)
            {
                //players_.insert(std::pair(player.first, player.second));
                players_[player.first] = PlayerRepr(player.second);
            }
        }

        [[nodiscard]] model::Game Restore() const {
            model::Game game {};
            for (int i = 0; i < maps_.size(); i++)
            {
                game.AddMap(maps_[i].Restore());
            }
            for (int i = 0; i < sessions_.size(); i++)
            {
                game.AddGameSession(sessions_[i].Restore());
            }
            for (auto& player : players_)
            {
                auto pl = player.second;
                model::PlayerTokens::GetInstance().AddPlayerWithToken(pl.Restore(), player.first);
            }            
            return game;
        }

        template <typename Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
            ar& maps_;
            ar& sessions_;
            ar& players_;
            ar& player_id_;
            ar& default_dog_speed_;
            ar& default_bag_capacity_;
            ar& loot_period_;
            ar& loot_probability_;
        }

    private:
        std::vector<MapRepr> maps_;

        std::unordered_map<std::string, PlayerRepr> players_;

        std::deque<GameSessionRepr> sessions_;

        int player_id_ = 0;
        double default_dog_speed_ = 1;
        double default_bag_capacity_ = 3;

        double loot_period_ = 5.0f;
        double loot_probability_ = .5f;
    };

/* Другие классы модели сериализуются и десериализуются похожим образом */

}  // namespace serialization
