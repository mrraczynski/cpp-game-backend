#pragma once

#include <boost/json.hpp>
#include "book.h"

namespace json = boost::json;
using namespace std::literals;

namespace json_parser {

    std::string GetResultJson(bool result);
    std::pair<Action, Book> ParseCommand(const std::string& jsn);
    std::string GetBooksJson(const std::vector<Book>& books);

}  // namespace json_loader
