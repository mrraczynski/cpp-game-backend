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

    std::vector<std::pair<std::string, std::string>> ShowAuthors() override;

private:
    pqxx::work& work_;
};

class BookRepositoryImpl : public domain::BookRepository {
public:
    explicit BookRepositoryImpl(pqxx::work& work)
        : work_{ work } {
    }

    void Save(const domain::Book& book) override;

    std::vector<std::pair<std::string, std::string>> ShowBooks() override;

    std::vector<std::pair<std::string, std::string>> ShowAuthorBooks(const std::string& id) override;

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