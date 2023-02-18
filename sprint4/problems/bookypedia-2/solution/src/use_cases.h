#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace app {

class UseCases {
public:
    virtual std::string AddAuthor(const std::string& name) = 0;
    virtual std::vector<std::pair<std::string, std::string>> ShowAuthors() = 0;
    virtual std::string GetAuthorIdByName(const std::string& name) = 0;
    virtual void DeleteAuthor(const std::string& author_id) = 0;
    virtual void UpdateAuthor(const std::string& author_id, const std::string& name) = 0;

    virtual std::string AddBook(const std::string& title, const std::string& author_id, int publication_year) = 0;
    virtual std::vector<std::tuple<std::string, std::string, std::string>> ShowBooks() = 0;
    virtual std::vector<std::pair<std::string, std::string>> ShowAuthorBooks(const std::string& id) = 0;
    virtual void SaveTag(const std::string& book_id, const std::string& tag) = 0;
    virtual void DeleteAllBookTags(const std::string& book_id) = 0;
    virtual void DeleteAuthorBooks(const std::string& author_id) = 0;
    virtual std::vector<std::string> ShowBook(std::string book_id) = 0;
    virtual std::vector<std::tuple<std::string, std::string, std::string>> GetBooksByTitle(const std::string& name) = 0;
    virtual void DeleteBook(const std::string& book_id) = 0;
    virtual void UpdateBook(const std::string& book_id, const std::string& title) = 0;
protected:
    ~UseCases() = default;
};

}  // namespace app
