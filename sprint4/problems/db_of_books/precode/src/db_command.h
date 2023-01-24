#pragma once

#include <pqxx/pqxx>
#include <vector>
#include "book.h"

using namespace std::literals;
// libpqxx использует zero-terminated символьные литералы вроде "abc"_zv;
using pqxx::operator"" _zv;

namespace db_command {

    constexpr auto create_books_table = "create_books_table"_zv;
    constexpr auto insert_book = "insert_book"_zv;

    class DBSession
    {
    public:
        DBSession() = delete;

        DBSession(std::string conn_str)
            : w_conn_(conn_str)
            , r_conn_(conn_str)
            , w_(w_conn_)
            , r_(r_conn_) {}

        void CreateBooksTable();

        bool AddBook(Book book);

        std::vector<Book> GetAllBooks();

        void SetCommit()
        {
            w_.commit();
        }

    private:

        pqxx::connection w_conn_;
        pqxx::connection r_conn_;
        pqxx::work w_;
        pqxx::read_transaction r_;
    };

}  // namespace json_loader
