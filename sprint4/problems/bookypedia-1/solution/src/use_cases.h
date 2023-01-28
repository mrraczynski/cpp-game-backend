#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace app {

class UseCases {
public:
    virtual void AddAuthor(const std::string& name) = 0;
    virtual std::unordered_map<std::string, std::string> ShowAuthors() = 0;
    virtual void AddBook(const std::string& title, const std::string& author_id, int publication_year) = 0;
    virtual std::vector<std::pair<std::string, std::string>> ShowBooks() = 0;
    virtual std::vector<std::pair<std::string, std::string>> ShowAuthorBooks(const std::string& id) = 0;
protected:
    ~UseCases() = default;
};

}  // namespace app
