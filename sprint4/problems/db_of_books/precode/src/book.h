#pragma once

#include <optional>
#include <string>

enum class Action
{
    add_book,
    get_all_books,
    exit
};

struct Book
{
    std::optional<int> id = std::nullopt;
    std::string title;
    std::string author;
    int year;
    std::optional<std::string> isbn = std::nullopt;
};
