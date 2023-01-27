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
    id uuid CONSTRAINT book_id_constraint NOT NULL,
    author_id uuid NOT NULL,
    title character varying(100) NOT NULL,
    publication_year integer
)
)"_zv);

    // коммитим изменения
    work_.commit();
}

}  // namespace postgres