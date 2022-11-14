#pragma once
#include <filesystem>
#include <iostream>
#include <fstream>
#include <boost/json.hpp>
#include "model.h"

namespace json = boost::json;
using namespace std::literals;

namespace json_loader {

model::Game LoadGame(const std::filesystem::path& json_path);

void GetErrorJson(std::string& jsn, const std::string& code, const std::string& message);
void GetAuthInfo(std::string& jsn, const std::string& token, const int& id);
void GetMapsJson(std::string& jsn, const model::Game& game);
void GetMapJson(std::string& jsn, const model::Map* game);
void GetMapJson(std::string& jsn, const model::Map* game);
std::string GetEmptyObject();
std::string CreatePlayersArray(std::vector<model::Player> players);
std::string CreatePlayersWithParametersArray(std::vector<model::Player> players);

}  // namespace json_loader
