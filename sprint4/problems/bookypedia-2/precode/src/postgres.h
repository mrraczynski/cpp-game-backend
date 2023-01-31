#pragma once
#include <pqxx/pqxx>

#include "author.h"
#include "book.h"

namespace postgres {

class AuthorRepositoryImpl : public domain::AuthorRepository {
public:
    explicit AuthorRepositoryImpl(pqxx::work& work)
        : work_{work} {
    }

    void Save(const domain::Author& author) override;

    std::unordered_map<std::string, std::string> ShowAuthors() override;

    std::string GetAuthorIdByName(const std::string& name) override;
    void DeleteAuthor(const std::string& author_id) override;
    void UpdateAuthor(const std::string& author_id, const std::string& name) override;

private:
    pqxx::work& work_;
};

class BookRepositoryImpl : public domain::BookRepository {
public:
    explicit BookRepositoryImpl(pqxx::work& work)
        : work_{ work } {
    }

    void Save(const domain::Book& book) override;

    std::vector<std::tuple<std::string, std::string, std::string>> ShowBooks() override;

    std::vector<std::pair<std::string, std::string>> ShowAuthorBooks(const std::string& id) override;

    void SaveTag(const std::string& book_id, const std::string& tag) override;

    void DeleteAllBookTags(const std::string& book_id) override;

    void DeleteAuthorBooks(const std::string& author_id) override;

    std::vector<std::string> ShowBook(std::string book_id) override;

    std::vector<std::tuple<std::string, std::string, std::string>> GetBooksByTitle(const std::string& name) override;

    void DeleteBook(const std::string& book_id) override;

    void UpdateBook(const std::string& book_id, const std::string& title) override;

private:
    pqxx::work& work_;
};

class Database {
public:
    explicit Database(pqxx::connection connection);

    AuthorRepositoryImpl& GetAuthors() & {
        return authors_;
    }

    BookRepositoryImpl& GetBooks()& {
        return books_;
    }

private:
    pqxx::connection connection_;
    pqxx::work work_{connection_};
    AuthorRepositoryImpl authors_{work_};
    BookRepositoryImpl books_{ work_ };
    
};

}  // namespace postgres