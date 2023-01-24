#include "db_command.h"
#define BOOST_BEAST_USE_STD_STRING_VIEW

namespace db_command {

    std::vector<Book> DBSession::GetAllBooks()
    {
        auto query_text = "SELECT id, title, author, year, \"ISBN\" FROM books ORDER BY year desc, title asc, author asc, \"ISBN\" asc"_zv;
        std::vector<Book> books;
        // ¬ыполн€ем запрос и итерируемс€ по строкам ответа
        for (auto& [id, title, author, year, isbn] : r_.query<std::optional<int>, std::string, std::string, int, std::optional<std::string>>(query_text))
        {
            books.push_back(Book{ id, title, author, year, isbn });
        }
        return books;
    }

    bool DBSession::AddBook(Book book)
    {
        try
        {
            w_conn_.prepare(insert_book, "INSERT INTO books (title, author, year, \"ISBN\") VALUES($1, $2, $3, $4); "_zv);
            if (book.isbn != std::nullopt)
            {
                w_.exec_prepared(insert_book, book.title, book.author, book.year, book.isbn.value());
            }
            else
            {
                w_.exec_prepared(insert_book, book.title, book.author, book.year, std::nullopt);
            }
            w_conn_.unprepare(insert_book);
            return true;
        }
        catch (std::exception& e)
        {
            return false;
        }
    }

    void DBSession::CreateBooksTable()
    {
        w_conn_.prepare(create_books_table, "CREATE TABLE IF NOT EXISTS books (id SERIAL PRIMARY KEY, title varchar(100) NOT NULL, author varchar(100) NOT NULL, year integer NOT NULL, \"ISBN\" char(13) UNIQUE); "_zv);
        w_.exec_prepared(create_books_table);
        w_conn_.unprepare(create_books_table);
    }


}  // namespace json_loader