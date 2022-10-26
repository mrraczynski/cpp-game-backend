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
void GetMapJson(std::string& jsn, const model::Map* game);

}  // namespace json_loader
