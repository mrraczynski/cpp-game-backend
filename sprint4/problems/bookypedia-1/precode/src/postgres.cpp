#include "postgres.h"

#include <pqxx/zview.hxx>

namespace postgres {

using namespace std::literals;
using pqxx::operator"" _zv;

void AuthorRepositoryImpl::Save(const domain::Author& author) {
    work_.exec_params(R"(
INSERT INTO authors (id, name) VALUES ($1, $2)
ON CONFLICT (id) DO UPDATE SET name=$2;
)"_zv,
                      author.GetId().ToString(), author.GetName());
    work_.commit();
}

std::unordered_map<std::string, std::string> AuthorRepositoryImpl::ShowAuthors() {
    auto query_text = R"(
SELECT id,
       name
  FROM authors
 ORDER BY name DESC;
)"_zv;
    auto result = work_.exec_params(query_text);
    std::unordered_map<std::string, std::string> authors;
    for (const auto& row : result) 
    {
        authors[row[0].c_str()] = row[1].c_str();
    }
    work_.commit();
    return authors;
}

void BookRepositoryImpl::Save(const domain::Book& book) {
    work_.exec_params(R"(
INSERT INTO books (id, author_id, title, publication_year) VALUES ($1, $2, $3, $4)
ON CONFLICT (id) DO UPDATE SET author_id=$2, title=$3, publication_year=$4;
)"_zv,
book.GetId().ToString(), book.GetAuthorId(), book.GetTitle(), book.GetPublicationYear());
    work_.commit();
}

std::vector<std::pair<std::string, std::string>> BookRepositoryImpl::ShowBooks()
{
    auto query_text = R"(
SELECT title,
       publication_year
  FROM books
 ORDER BY title ASC;
)"_zv;
    auto result = work_.exec_params(query_text);
    std::vector<std::pair<std::string, std::string>> authors;
    for (const auto& row : result)
    {
        authors.push_back({ row[0].c_str(), row[1].c_str() });
    }
    work_.commit();
    return authors;
}

std::vector<std::pair<std::string, std::string>> BookRepositoryImpl::ShowAuthorBooks(const std::string& id)
{
    auto query_text = R"(
SELECT title,
       publication_year
  FROM books
 WHERE author_id = $1
 ORDER BY publication_year ASC;
)"_zv;
    auto result = work_.exec_params(query_text, id);
    std::vector<std::pair<std::string, std::string>> authors;
    for (const auto& row : result)
    {
        authors.push_back({ row[0].c_str(), row[1].c_str() });
    }
    work_.commit();
    return authors;
}

Database::Database(pqxx::connection connection)
    : connection_{std::move(connection)} {
    work_.exec(R"(
CREATE TABLE IF NOT EXISTS authors (
    id UUID CONSTRAINT author_id_constraint PRIMARY KEY,
    name varchar(100) UNIQUE NOT NULL
);
)"_zv);
    work_.exec(R"(
CREATE TABLE IF NOT EXISTS books
(
    id uuid CONSTRAINT book_id_constraint PRIMARY KEY,
    author_id uuid NOT NULL,
    title varchar(100) NOT NULL,
    publication_year integer
)
)"_zv);

    // коммитим изменения
    work_.commit();
}

}  // namespace postgres