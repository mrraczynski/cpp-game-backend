#include "json_loader.h"
#define BOOST_BEAST_USE_STD_STRING_VIEW

namespace json_loader {

    void GetErrorJson(std::string& jsn, const std::string& code, const std::string& message)
    {
        json::object error_json;
        error_json["code"] = code;
        error_json["message"] = message;
        jsn = json::serialize(error_json);
    }

    void GetMapsJson(std::string& jsn, const model::Game& game)
    {
        json::array maps_json;
        
        for (auto map : game.GetMaps())
        {
            json::object obj;
            obj["id"] = *map.GetId();
            obj["name"] = map.GetName();
            maps_json.emplace_back(obj);
        }
        jsn = json::serialize(maps_json);
    }

    void GetMapJson(std::string& jsn, const model::Map* map)
    {        
        json::object map_obj;
        map_obj["id"] = *map->GetId();
        map_obj["name"] = map->GetName();
        {
            json::array roads_json;
            for (auto road : map->GetRoads())
            {
                model::Point start = road.GetStart();
                model::Point end = road.GetEnd();
                if (road.IsHorizontal())
                {
                    json::object road_obj;
                    road_obj["x0"] = start.x;
                    road_obj["y0"] = start.y;
                    road_obj["x1"] = end.x;
                    roads_json.emplace_back(road_obj);
                }
                else if (road.IsVertical())
                {
                    json::object road_obj;
                    road_obj["x0"] = start.x;
                    road_obj["y0"] = start.y;
                    road_obj["y1"] = end.y;
                    roads_json.emplace_back(road_obj);
                }
            }
            map_obj["roads"] = roads_json;
        }
        {
            json::array buildings_json;
            for (auto building : map->GetBuildings())
            {
                json::object road_obj;
                model::Rectangle bounds = building.GetBounds();
                road_obj["x"] = bounds.position.x;
                road_obj["y"] = bounds.position.y;
                road_obj["w"] = bounds.size.width;
                road_obj["h"] = bounds.size.height;
                buildings_json.emplace_back(road_obj);
            }
            map_obj["buildings"] = buildings_json;
        }
        {
            json::array offices_json;
            for (auto office : map->GetOffices())
            {
                json::object office_obj;
                office_obj["id"] = *office.GetId();
                office_obj["x"] = office.GetPosition().x;
                office_obj["y"] = office.GetPosition().y;
                office_obj["offsetX"] = office.GetOffset().dx;
                office_obj["offsetY"] = office.GetOffset().dy;
                offices_json.emplace_back(office_obj);
            }
            map_obj["offices"] = offices_json;
        }
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
            std::cerr << "json_loader.cpp/GetJson: " << e.what() << std::endl;
        }
    }

    void ParseOffices(model::Map& map, const json::array& offices_arr)
    {
        try
        {
            for (int y = 0; y < offices_arr.size(); y++)
            {
                const auto& building_obj = offices_arr[y].as_object();
                model::Office office(model::Office::Id(std::string(building_obj.at("id").as_string())),
                    model::Point{ building_obj.at("x").to_number<int>(), building_obj.at("y").to_number<int>() },
                    model::Offset{ building_obj.at("offsetX").to_number<int>(), building_obj.at("offsetY").to_number<int>() });
                map.AddOffice(office);
            }
        }
        catch (std::exception& e)
        {
            std::cerr << "json_loader.cpp/ParseBuildings: " << e.what() << std::endl;
        }
    }

    void ParseBuildings(model::Map& map, const json::array& buildings_arr)
    {
        try
        {
            for (int y = 0; y < buildings_arr.size(); y++)
            {
                const auto& building_obj = buildings_arr[y].as_object();
                model::Building building(model::Rectangle{ model::Point { building_obj.at("x").to_number<int>(), building_obj.at("y").to_number<int>() },
                   model::Size { building_obj.at("w").to_number<int>(), building_obj.at("h").to_number<int>() } });
                map.AddBuilding(building);
            }
        }
        catch (std::exception& e)
        {
            std::cerr << "json_loader.cpp/ParseBuildings: " << e.what() << std::endl;
        }
    }

    void ParseRoads(model::Map& map, const json::array& roads_arr)
    {
        try
        {
            for (int y = 0; y < roads_arr.size(); y++)
            {
                const auto& road_obj = roads_arr[y].as_object();
                if (road_obj.if_contains("x1"))
                {
                    model::Road road(model::Road::HORIZONTAL, model::Point{ road_obj.at("x0").to_number<int>(), road_obj.at("y0").to_number<int>() }, road_obj.at("x1").to_number<int>());
                    map.AddRoad(road);
                }
                else if (road_obj.if_contains("y1"))
                {
                    model::Road road(model::Road::VERTICAL, model::Point{ road_obj.at("x0").to_number<int>(), road_obj.at("y0").to_number<int>() }, road_obj.at("y1").to_number<int>());
                    map.AddRoad(road);
                }
            }
        }
        catch (std::exception& e)
        {
            std::cerr << "json_loader.cpp/ParseRoads: " << e.what() << std::endl;
        }
    }

    model::Game LoadGame(const std::filesystem::path& json_path) {
        // Загрузить содержимое файла json_path, например, в виде строки
        // Распарсить строку как JSON, используя boost::json::parse
        // Загрузить модель игры из файла
        model::Game game;
        try
        {
            std::string jsn;
            GetJson(jsn, json_path);
            auto value = json::parse(jsn);
            const auto& maps_arr = value.as_object().at("maps").as_array();
            
            for (int i = 0; i < maps_arr.size(); i++)
            {
                const auto& maps_obj = maps_arr[i].as_object();
                model::Map map(model::Map::Id(std::string(maps_obj.at("id").as_string())), std::string(maps_obj.at("name").as_string()));
                const auto& roads_arr = maps_obj.at("roads").as_array();
                ParseRoads(map, roads_arr);
                const auto& buildings_arr = maps_obj.at("buildings").as_array();
                ParseBuildings(map, buildings_arr);
                const auto& offices_arr = maps_obj.at("offices").as_array();
                ParseOffices(map, offices_arr);
                game.AddMap(map);
            }
        }
        catch (std::exception& e)
        {
            std::cerr << "json_loader.cpp/LoadGame: " << e.what() << std::endl;
        }
        return game;
    }

}  // namespace json_loader
