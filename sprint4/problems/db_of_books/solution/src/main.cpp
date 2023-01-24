// main.cpp

#include <iostream>
#include <pqxx/pqxx>

#include "json_parser.h"
#include "db_command.h"

using namespace std::literals;
// libpqxx использует zero-terminated символьные литералы вроде "abc"_zv;
using pqxx::operator"" _zv;
namespace db = db_command;

int main(int argc, const char* argv[]) {
    try {
        if (argc == 1) {
            std::cout << "Usage: connect_db <conn-string>\n"sv;
            return EXIT_SUCCESS;
        }
        else if (argc != 2) {
            std::cerr << "Invalid command line\n"sv;
            return EXIT_FAILURE;
        }

        db::DBSession db_session{ argv[1] };
        db_session.CreateBooksTable();
        db_session.SetCommit();

        std::string command_str;
        bool is_running = true;
        while (is_running)
        {
            std::getline(std::cin, command_str);

            auto command = json_parser::ParseCommand(command_str);

            switch (command.first)
            {
            case Action::add_book:
                {
                    bool result = db_session.AddBook(command.second);
                    db_session.SetCommit();
                    std::cout << json_parser::GetResultJson(result);
                }
                break;
            case Action::get_all_books:
                {
                    auto books = db_session.GetAllBooks();
                    std::cout << json_parser::GetBooksJson(books);
                }
                break;
            case Action::exit:
                is_running = false;
                break;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}