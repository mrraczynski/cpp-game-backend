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

std::vector<std::pair<std::string, std::string>> AuthorRepositoryImpl::ShowAuthors() {
    auto query_text = R"(
SELECT id,
       name
  FROM authors
 ORDER BY name DESC;
)"_zv;
    auto result = work_.exec_params(query_text);
    std::vector<std::pair<std::string, std::string>> authors;
    for (const auto& row : result) 
    {
        authors.push_back({ row[0].c_str() , row[1].c_str() });
    }
    work_.commit();
    return authors;
}

std::string AuthorRepositoryImpl::GetAuthorIdByName(const std::string& name)
{
    auto query_text = R"(
SELECT id       
  FROM authors
 WHERE name like $1
 ORDER BY name ASC;
)"_zv;
    auto result = work_.exec_params(query_text, name);
    work_.commit();
    for (const auto& row : result)
    {
        return row[0].c_str();
    }
    return std::string();
}

void AuthorRepositoryImpl::DeleteAuthor(const std::string& author_id)
{
    work_.exec_params(R"(
DELETE 
  FROM tags
 WHERE book_id in (SELECT book_id
                     FROM books
                    WHERE author_id = $1);
)"_zv,
author_id);
    work_.exec_params(R"(
DELETE 
  FROM books
 WHERE author_id = $1;
)"_zv,
author_id);
    work_.exec_params(R"(
DELETE 
  FROM authors
 WHERE id = $1;
)"_zv,
author_id);
    work_.commit();
}

void AuthorRepositoryImpl::UpdateAuthor(const std::string& author_id, const std::string& name)
{
    work_.exec_params(R"(
UPDATE authors
   SET name = $2
 WHERE id = $1;
)"_zv,
author_id, name);
    work_.commit();
}

void BookRepositoryImpl::Save(const domain::Book& book) {
    work_.exec_params(R"(
INSERT INTO books (id, author_id, title, publication_year) VALUES ($1, $2, $3, $4)
ON CONFLICT (id) DO UPDATE SET author_id=$2, title=$3, publication_year=$4;
)"_zv,
book.GetId().ToString(), book.GetAuthorId(), book.GetTitle(), book.GetPublicationYear());
    work_.commit();
}

std::vector<std::tuple<std::string, std::string, std::string>> BookRepositoryImpl::GetBooksByTitle(const std::string& name)
{
    auto query_text = R"(
SELECT books.title,
       books.publication_year,
       authors.name,
       books.id
  FROM books
  JOIN authors
    ON books.author_id = authors.id
 WHERE books.title like $1
 ORDER BY books.title ASC, authors.name ASC, books.publication_year ASC;
)"_zv;
    auto result = work_.exec_params(query_text, name);
    work_.commit();
    std::vector<std::tuple<std::string, std::string, std::string>> books;
    for (const auto& row : result)
    {
        std::string title_author{ row[0].c_str() };
        title_author = title_author + " by "s + std::string{ row[2].c_str() };
        books.push_back({ title_author, row[1].c_str(), row[3].c_str() });
    }
    return books;
}

std::vector<std::tuple<std::string, std::string, std::string>> BookRepositoryImpl::ShowBooks()
{
    auto query_text = R"(
SELECT books.title,
       books.publication_year,
       authors.name,
       books.id
  FROM books
  JOIN authors
    ON books.author_id = authors.id
 ORDER BY books.title ASC, authors.name ASC, books.publication_year ASC;
)"_zv;
    auto result = work_.exec_params(query_text);
    work_.commit();
    std::vector<std::tuple<std::string, std::string, std::string>> books;
    for (const auto& row : result)
    {
        std::string title_author{ row[0].c_str() };
        title_author = title_author + " by "s + std::string{ row[2].c_str() };
        books.push_back({ title_author, row[1].c_str(), row[3].c_str() });
    }
    return books;
}

std::vector<std::string> BookRepositoryImpl::ShowBook(std::string book_id)
{
    auto query_text = R"(
SELECT books.title,
       authors.name,
       books.publication_year,
	   string_agg(tags.tag, ', ') AS tags_list
  FROM books
  JOIN authors
    ON books.author_id = authors.id
  JOIN tags
    on tags.book_id = books.id
 WHERE books.id = $1
 GROUP BY books.title, books.publication_year, authors.name, books.id;
)"_zv;
    auto result = work_.exec_params(query_text, book_id);
    work_.commit();
    std::vector<std::string> authors;
    for (const auto& row : result)
    {
        authors.push_back(row[0].c_str());
        authors.push_back(row[1].c_str());
        authors.push_back(row[2].c_str());
        authors.push_back(row[3].c_str());
    }
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

void BookRepositoryImpl::SaveTag(const std::string& book_id, const std::string& tag) {
    work_.exec_params(R"(
INSERT INTO tags (book_id, tag) VALUES ($1, $2)
ON CONFLICT (book_id, tag) DO NOTHING;
)"_zv,
book_id, tag);
    work_.commit();
}

void BookRepositoryImpl::DeleteAllBookTags(const std::string& book_id) {
    work_.exec_params(R"(
DELETE 
  FROM tags
 WHERE book_id = $1;
)"_zv,
book_id);
work_.commit();
}

void BookRepositoryImpl::DeleteAuthorBooks(const std::string& author_id)
{
    work_.exec_params(R"(
DELETE 
  FROM books
 WHERE author_id = $1;
)"_zv,
author_id);
    work_.commit();
}

void BookRepositoryImpl::DeleteBook(const std::string& book_id)
{
    work_.exec_params(R"(
DELETE 
  FROM tags
 WHERE book_id = $1;
)"_zv,
book_id);
    work_.exec_params(R"(
DELETE 
  FROM books
 WHERE id = $1;
)"_zv,
book_id);
    work_.commit();
}

void BookRepositoryImpl::UpdateBook(const std::string& book_id, const std::string& title)
{
    work_.exec_params(R"(
UPDATE books
   SET title = $2
 WHERE id = $1;
)"_zv,
book_id, title);
    work_.commit();
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
    work_.exec(R"(
CREATE TABLE IF NOT EXISTS tags
(
    book_id uuid NOT NULL,
    tag varchar(30) NOT NULL,
    CONSTRAINT tags_constraint PRIMARY KEY (book_id, tag)
)
)"_zv);

    // коммитим изменения
    work_.commit();
}

}  // namespace postgres