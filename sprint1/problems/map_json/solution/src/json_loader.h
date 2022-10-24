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
void GetMapsJson(std::string& jsn, const model::Game& game);
void GetMapJsonById(std::string& jsn, const model::Game& game, const std::string& map_id);

}  // namespace json_loader
