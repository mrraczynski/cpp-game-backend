#include "json_loader.h"
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

    void GetMapJson(std::string& jsn, const model::Map* map)
    {
        json::object map_obj;
        map_obj[ID] = *map->GetId();
        map_obj[NAME] = map->GetName();
        {
            json::array roads_json;
            for (auto road : map->GetRoads())
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
            for (auto building : map->GetBuildings())
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
            for (auto office : map->GetOffices())
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
                model::Office office(model::Office::Id(std::string(building_obj.at(ID).as_string())),
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
            const auto& maps_arr = value.as_object().at(MAPS).as_array();

            for (int map_inc = 0; map_inc < maps_arr.size(); ++map_inc)
            {
                const auto& maps_obj = maps_arr[map_inc].as_object();
                model::Map map(model::Map::Id(std::string(maps_obj.at(ID).as_string())), std::string(maps_obj.at(NAME).as_string()));

                const auto& roads_arr = maps_obj.at(ROADS).as_array();
                ParseRoads(map, roads_arr);

                const auto& buildings_arr = maps_obj.at(BUILDINGS).as_array();
                ParseBuildings(map, buildings_arr);

                const auto& offices_arr = maps_obj.at(OFFICES).as_array();
                ParseOffices(map, offices_arr);

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