#include <string>
#include "json_parser.h"
#define BOOST_BEAST_USE_STD_STRING_VIEW

namespace json_parser {

    std::string GetResultJson(bool result)
    {
        json::object res_obj;
        res_obj["result"] = result;
        return json::serialize(res_obj);
    }

    std::string GetBooksJson(const std::vector<Book>& books)
    {
        json::array books_arr;
        for (auto& book : books)
        {
            json::object book_obj;
            book_obj["id"] = book.id.has_value() ? book.id.value() : -1;
            book_obj["title"] = book.title;
            book_obj["author"] = book.author;
            book_obj["year"] = book.year;
            if (book.isbn.has_value())
            {
                //book_obj["ISBN"] = book.isbn.has_value() ? book.isbn.value() : nullptr;
                book_obj["ISBN"] = book.isbn.value();
            }
            else
            {
                book_obj["ISBN"] = nullptr;
            }
            books_arr.push_back(book_obj);
        }
        return json::serialize(books_arr);
    }

    std::pair<Action, Book> ParseCommand(const std::string& jsn)
    {
        auto value = json::parse(jsn);

        Action action;

        if (auto it = value.as_object().find("action"); it != value.as_object().end())
        {
            auto action_str = it->value().as_string().data();
            if (action_str == "add_book"s)
            {
                action = Action::add_book;
            }
            else if (action_str == "all_books"s)
            {
                action = Action::get_all_books;
            }
            else if (action_str == "exit"s)
            {
                action = Action::exit;
            }
        }

        Book book;

        if (auto it = value.as_object().find("payload"); it != value.as_object().end())
        {
            auto& payload_obj = it->value().as_object();            
            if (auto it_payload = payload_obj.find("title"); it_payload != payload_obj.end())
            {
                book.title = it_payload->value().as_string().data();
            }
            if (auto it_payload = payload_obj.find("author"); it_payload != payload_obj.end())
            {
                book.author = it_payload->value().as_string().data();
            }
            if (auto it_payload = payload_obj.find("year"); it_payload != payload_obj.end())
            {
                book.year = it_payload->value().as_int64();
            }
            if (auto it_payload = payload_obj.find("ISBN"); it_payload != payload_obj.end())
            {
                if (!it_payload->value().is_null())
                {
                    book.isbn = it_payload->value().as_string().data();
                }
            }
        }

        return std::pair{ action, book };
    }

}  // namespace json_loader