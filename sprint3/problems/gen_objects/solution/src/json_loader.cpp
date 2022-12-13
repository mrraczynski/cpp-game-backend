#include "json_loader.h"
#include <cmath>
#define BOOST_BEAST_USE_STD_STRING_VIEW

namespace json_loader {

    constexpr char ERROR_CODE[] = "code";
    constexpr char ERROR_MESSAGE[] = "message";
    constexpr char ID[] = "id";
    constexpr char NAME[] = "name";
    constexpr char MAPS[] = "maps";
    constexpr char ROADS[] = "roads";
    constexpr char BUILDINGS[] = "buildings";
    constexpr char OFFICES[] = "offices";
    constexpr char X[] = "x";
    constexpr char Y[] = "y";
    constexpr char W[] = "w";
    constexpr char H[] = "h";
    constexpr char X0[] = "x0";
    constexpr char Y0[] = "y0";
    constexpr char X1[] = "x1";
    constexpr char Y1[] = "y1";
    constexpr char OFFSET_X[] = "offsetX";
    constexpr char OFFSET_Y[] = "offsetY";
    constexpr char AUTH_TOKEN[] = "authToken";
    constexpr char PLAYER_ID[] = "playerId";
    constexpr char PLAYER_NAME[] = "name";
    constexpr char PLAYERS[] = "players";

    model::Point MakePoint(json::value x, json::value y)
    {
        return model::Point{ x.to_number<int>(), y.to_number<int>() };
    }

    void GetErrorJson(std::string& jsn, const std::string& code, const std::string& message)
    {
        json::object error_json;
        error_json[ERROR_CODE] = code;
        error_json[ERROR_MESSAGE] = message;
        jsn = json::serialize(error_json);
    }

    std::string GetEmptyObject()
    {
        return std::string(json::serialize(json::object()));
    }

    void GetAuthInfo(std::string& jsn, const std::string& token, const int& id)
    {
        json::object auth_json;
        auth_json[AUTH_TOKEN] = token;
        auth_json[PLAYER_ID] = id;
        jsn = json::serialize(auth_json);
    }

    void GetMapsJson(std::string& jsn, const model::Game& game)
    {
        json::array maps_json;

        for (auto map : game.GetMaps())
        {
            json::object obj;
            obj[ID] = *map.GetId();
            obj[NAME] = map.GetName();
            maps_json.emplace_back(obj);
        }
        jsn = json::serialize(maps_json);
    }

    json::object GetLostObjectsJson(const model::Map* map)
    {
        json::object loot_json;
        int inc = 0;
        for (auto& obj : map->GetLootObjects())
        {
            json::object loot_obj;
            loot_obj["type"] = obj.loot_type.type_num;
            json::array loot_pos_arr;
            loot_pos_arr.emplace_back(obj.position.x);
            loot_pos_arr.emplace_back(obj.position.y);
            loot_obj["pos"] = loot_pos_arr;
            loot_json[std::to_string(obj.id)] = loot_obj;
        }
        return loot_json;
    }

    std::string CreatePlayersWithParametersArray(std::vector<model::Player> players, const model::Map* map)
    {
        json::object players_json;
        for (auto& player : players)
        {
            double precision = 0.01;
            model::Dog& dog = player.GetDog();
            json::object player_json;
            {
                double x = std::round(dog.GetPosition().x / precision) * precision;
                double y = std::round(dog.GetPosition().y / precision) * precision;
                std::ostringstream ss;
                ss << "[";
                ss << std::setprecision(3) << x;
                ss << ",";
                ss << std::setprecision(3) << y;
                ss << "]";
                json::value pos_arr = json::parse(ss.str());
                player_json["pos"] = pos_arr;
            }
            {
                double x = std::round(dog.GetSpeed().x / precision) * precision;
                double y = std::round(dog.GetSpeed().y / precision) * precision;
                std::ostringstream ss;
                ss << "[";
                ss << std::setprecision(3) << x;
                ss << ",";
                ss << std::setprecision(3) << y;
                ss << "]";
                json::value speed_arr = json::parse(ss.str());
                player_json["speed"] = speed_arr;
            }
            std::string str = std::string(model::Directions[dog.GetDirection()]);
            player_json["dir"] = std::string(model::Directions[dog.GetDirection()]);
            players_json[std::to_string(player.GetId())] = player_json;
        }
        json::object final_json;
        final_json[PLAYERS] = players_json;
        final_json["lostObjects"] = GetLostObjectsJson(map);
        return json::serialize(final_json);
    }

    std::string CreatePlayersArray(std::vector<model::Player> players)
    {
        json::object players_json;
        for (auto& player : players)
        {
            json::object player_json;
            player_json[PLAYER_NAME] = player.GetName();
            players_json[std::to_string(player.GetId())] = player_json;
        }
        return json::serialize(players_json);
    }

    json::array GetMapLootTypesJson(const model::Map* map)
    {
        json::array loot_types_json;
        /*for (auto& loot_type : map->GetLootTypes())
        {
            json::object loot_type_json;
            loot_type_json[NAME] = loot_type.name;
            loot_type_json["file"] = loot_type.file;
            loot_type_json["type"] = loot_type.type;
            loot_type_json["rotation"] = loot_type.rotation;
            loot_type_json["color"] = loot_type.color;
            loot_type_json["scale"] = loot_type.scale;
            loot_types_json.emplace_back(loot_type_json);
        }*/
        for (auto& loot_type : map->GetLootTypesRaw())
        {
            loot_types_json.emplace_back(json::parse(loot_type));
        }
        return loot_types_json;
    }

    void GetMapJson(std::string& jsn, const model::Map* map)
    {
        json::object map_obj;
        map_obj[ID] = *map->GetId();
        map_obj[NAME] = map->GetName();
        {
            json::array roads_json;
            for (auto& road : map->GetRoads())
            {
                model::Point start = road.GetStart();
                model::Point end = road.GetEnd();
                if (road.IsHorizontal())
                {
                    json::object road_obj;
                    road_obj[X0] = start.x;
                    road_obj[Y0] = start.y;
                    road_obj[X1] = end.x;
                    roads_json.emplace_back(road_obj);
                }
                else if (road.IsVertical())
                {
                    json::object road_obj;
                    road_obj[X0] = start.x;
                    road_obj[Y0] = start.y;
                    road_obj[Y1] = end.y;
                    roads_json.emplace_back(road_obj);
                }
            }
            map_obj[ROADS] = roads_json;
        }
        {
            json::array buildings_json;
            for (auto& building : map->GetBuildings())
            {
                json::object road_obj;
                model::Rectangle bounds = building.GetBounds();
                road_obj[X] = bounds.position.x;
                road_obj[Y] = bounds.position.y;
                road_obj[W] = bounds.size.width;
                road_obj[H] = bounds.size.height;
                buildings_json.emplace_back(road_obj);
            }
            map_obj[BUILDINGS] = buildings_json;
        }
        {
            json::array offices_json;
            for (auto& office : map->GetOffices())
            {
                json::object office_obj;
                office_obj[ID] = *office.GetId();
                office_obj[X] = office.GetPosition().x;
                office_obj[Y] = office.GetPosition().y;
                office_obj[OFFSET_X] = office.GetOffset().dx;
                office_obj[OFFSET_Y] = office.GetOffset().dy;
                offices_json.emplace_back(office_obj);
            }
            map_obj[OFFICES] = offices_json;
        }
        map_obj["lootTypes"] = GetMapLootTypesJson(map);
        jsn = json::serialize(map_obj);
    }

    void GetJson(std::string& jsn, const std::filesystem::path& json_path)
    {
        try
        {
            std::ifstream file(json_path);
            std::stringstream buffer;
            buffer << file.rdbuf();
            jsn = std::move(buffer.str());
        }
        catch (std::exception& e)
        {
            std::cerr << "json_loader.cpp/GetJson: "sv << e.what() << std::endl;
        }
    }

    void ParseOffices(model::Map& map, const json::array& offices_arr)
    {
        try
        {
            for (int office_inc = 0; office_inc < offices_arr.size(); ++office_inc)
            {
                const auto& building_obj = offices_arr[office_inc].as_object();
                model::Office office(model::Office::Id(std::string(building_obj.at(ID).as_string().data())),
                    MakePoint(building_obj.at(X), building_obj.at(Y)),
                    model::Offset{ building_obj.at(OFFSET_X).to_number<int>(), building_obj.at(OFFSET_Y).to_number<int>() });
                map.AddOffice(office);
            }
        }
        catch (std::exception& e)
        {
            std::cerr << "json_loader.cpp/ParseBuildings: "sv << e.what() << std::endl;
        }
    }

    void ParseBuildings(model::Map& map, const json::array& buildings_arr)
    {
        try
        {
            for (int building_inc = 0; building_inc < buildings_arr.size(); ++building_inc)
            {
                const auto& building_obj = buildings_arr[building_inc].as_object();
                model::Building building(model::Rectangle{ MakePoint(building_obj.at(X), building_obj.at(Y)),
                   model::Size { building_obj.at(W).to_number<int>(), building_obj.at(H).to_number<int>() } });
                map.AddBuilding(building);
            }
        }
        catch (std::exception& e)
        {
            std::cerr << "json_loader.cpp/ParseBuildings: "sv << e.what() << std::endl;
        }
    }

    void ParseRoads(model::Map& map, const json::array& roads_arr)
    {
        try
        {
            for (int road_inc = 0; road_inc < roads_arr.size(); ++road_inc)
            {
                const auto& road_obj = roads_arr[road_inc].as_object();
                if (road_obj.if_contains(X1))
                {
                    model::Road road(model::Road::HORIZONTAL, MakePoint(road_obj.at(X0), road_obj.at(Y0)), road_obj.at(X1).to_number<int>());
                    map.AddRoad(road);
                }
                else if (road_obj.if_contains(Y1))
                {
                    model::Road road(model::Road::VERTICAL, MakePoint(road_obj.at(X0), road_obj.at(Y0)), road_obj.at(Y1).to_number<int>());
                    map.AddRoad(road);
                }
            }
        }
        catch (std::exception& e)
        {
            std::cerr << "json_loader.cpp/ParseRoads: "sv << e.what() << std::endl;
        }
    }

    model::Game LoadGame(const std::filesystem::path& json_path, bool randomize_spawn_points) {
        // Загрузить содержимое файла json_path, например, в виде строки
        // Распарсить строку как JSON, используя boost::json::parse
        // Загрузить модель игры из файла
        model::Game game;
        try
        {
            std::string jsn;
            GetJson(jsn, json_path);
            auto value = json::parse(jsn);
            const auto& maps_arr = value.as_object().at(MAPS).as_array();

            if (auto it = value.as_object().find("lootGeneratorConfig"); it != value.as_object().end())
            {
                game.SetDogSpeed(it->value().as_object().at("period").as_double());
                game.SetLootProbability(it->value().as_object().at("probability").as_double());
            }

            if (auto it = value.as_object().find("defaultDogSpeed"); it != value.as_object().end())
            {
                game.SetLootPeriod(it->value().as_double());
            }

            for (int map_inc = 0; map_inc < maps_arr.size(); ++map_inc)
            {
                const auto& maps_obj = maps_arr[map_inc].as_object();
                model::Map map(model::Map::Id(std::string(maps_obj.at(ID).as_string().data())), std::string(maps_obj.at(NAME).as_string().data()));
                map.SetRandomizeSpawnPoints(randomize_spawn_points);
                if (auto it = maps_obj.find("dogSpeed"); it != maps_obj.end())
                {
                    map.SetDogSpeed(it->value().as_double());                    
                }

                if (auto it = maps_obj.find("lootTypes"); it != maps_obj.end())
                {
                    auto& loot_types = maps_obj.at("lootTypes").as_array();
                    for (int lt_inc = 0; lt_inc < loot_types.size(); ++lt_inc)
                    {
                        auto& loot_type = loot_types[lt_inc].as_object();
                        model::LootType lt_struct{ loot_type.at("name").as_string().data(),
                            loot_type.find("file") != loot_type.end() ? loot_type.at("file").as_string().data() : std::string(),
                            loot_type.find("type") != loot_type.end() ? loot_type.at("type").as_string().data() : std::string(),
                            loot_type.find("rotation") != loot_type.end() ? loot_type.at("rotation").as_int64() : 0,
                            loot_type.find("color") != loot_type.end() ? loot_type.at("color").as_string().data() : std::string(),
                            loot_type.find("scale") != loot_type.end() ?  loot_type.at("scale").as_double() : 0 };
                        map.AddLootType(lt_struct, json::serialize(loot_type));
                    }
                }

                const auto& roads_arr = maps_obj.at(ROADS).as_array();
                ParseRoads(map, roads_arr);

                const auto& buildings_arr = maps_obj.at(BUILDINGS).as_array();
                ParseBuildings(map, buildings_arr);

                const auto& offices_arr = maps_obj.at(OFFICES).as_array();
                ParseOffices(map, offices_arr);

                //TODO: определить логику создания сессий в другой части системы
                model::GameSession session(model::GameSession::Id(std::string(maps_obj.at(ID).as_string().data())), map.GetId(), map.GetDogSpeed());
                game.AddGameSession(session);

                game.AddMap(map);
            }
        }
        catch (std::exception& e)
        {
            std::cerr << "json_loader.cpp/LoadGame: "sv << e.what() << std::endl;
        }
        return game;
    }

}  // namespace json_loader