#include <catch2/catch_test_macros.hpp>

#include "../src/model.h"

using namespace std::literals;

SCENARIO("Loot creation") {
    model::Game game;
    game.SetLootProbability(0.5);
    game.SetLootPeriod(5);
    constexpr model::TimeInterval TIME_INTERVAL = 1s;

    GIVEN("game model for loot creation") {
        WHEN("there is no map") {
            THEN("no loot created")
            {
                CHECK_NOTHROW(game.GenerateLoot(TIME_INTERVAL));
            }
        }
    }

    GIVEN("game model for loot creation") {
        model::Map map(model::Map::Id("map1"), "Map 1");
        game.AddMap(map);
        WHEN("there is a map, but no roads and no loot types") {
            THEN("no loot created")
            {
                CHECK_NOTHROW(game.GenerateLoot(TIME_INTERVAL));
                CHECK(game.GetMaps()[0].GetCurrentLootCount() == 0);
            }
        }
    }

    GIVEN("game model for loot creation") {
        model::Map map(model::Map::Id("map1"), "Map 1");
        model::LootType lt_struct{ "name", "file", "type", 90, "color", 1 };
        map.AddLootType(lt_struct, "{\"name\":\"key\",\"file\":\"assets / key.obj\",\"type\":\"obj\",\"scale\":3E-2}"s);
        game.AddMap(map);
        WHEN("there is a map, but no roads") {
            THEN("no loot created")
            {
                CHECK_NOTHROW(game.GenerateLoot(TIME_INTERVAL));
                CHECK(game.GetMaps()[0].GetCurrentLootCount() == 0);
            }
        }
    }

    GIVEN("game model for loot creation") {
        model::Map map(model::Map::Id("map1"), "Map 1");
        model::Road road(model::Road::HORIZONTAL, model::Point(1, 5), 2);
        map.AddRoad(road);
        game.AddMap(map);
        WHEN("there is a map, but no loot types") {
            THEN("no loot created")
            {
                CHECK_NOTHROW(game.GenerateLoot(TIME_INTERVAL));
                CHECK(game.GetMaps()[0].GetCurrentLootCount() == 0);
            }
        }
    }

    GIVEN("game model for loot creation") {
        model::Map map(model::Map::Id("map1"), "Map 1");
        model::LootType lt_struct{ "name", "file", "type", 90, "color", 1 };
        map.AddLootType(lt_struct, "{\"name\":\"key\",\"file\":\"assets / key.obj\",\"type\":\"obj\",\"scale\":3E-2}"s);
        model::Road road(model::Road::HORIZONTAL, model::Point(1, 5), 2);
        map.AddRoad(road);
        game.AddMap(map);
        WHEN("there is a map with roads and loot types, but without a player") {
            THEN("there is loot created")
            {
                CHECK_NOTHROW(game.GenerateLoot(TIME_INTERVAL));
                CHECK(game.GetMaps()[0].GetCurrentLootCount() == 0);
            }
        }
    }

    GIVEN("game model for loot creation") {
        model::Map map(model::Map::Id("map1"), "Map 1");
        model::LootType lt_struct{ "name", "file", "type", 90, "color", 1 };
        map.AddLootType(lt_struct, "{\"name\":\"key\",\"file\":\"assets / key.obj\",\"type\":\"obj\",\"scale\":3E-2}"s);
        model::Road road(model::Road::HORIZONTAL, model::Point(1, 5), 2);
        map.AddRoad(road);
        model::GameSession session(model::GameSession::Id("map1"), model::Map::Id("map1"), 2);
        model::Player player(1, "player", model::Vector2{ 1, 5 }, &session);
        model::PlayerTokens::GetInstance().AddPlayer(player);
        game.AddGameSession(session);
        game.AddMap(map);
        WHEN("there is a map with roads, loot types and player") {
            THEN("there is loot created")
            {
                CHECK_NOTHROW(game.GenerateLoot(TIME_INTERVAL));
                CHECK(game.GetMaps()[0].GetCurrentLootCount() == 1);
            }
        }
    }
}

