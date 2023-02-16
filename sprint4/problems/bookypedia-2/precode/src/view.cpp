#include "view.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp> 
#include <iostream>

#include "menu.h"
#include "use_cases.h"

using namespace std::literals;
namespace ph = std::placeholders;

namespace ui {

View::View(menu::Menu& menu, app::UseCases& use_cases, std::istream& input, std::ostream& output)
    : menu_{menu}
    , use_cases_{use_cases}
    , input_{input}
    , output_{output} {
    menu_.AddAction(  //
        "AddAuthor"s, "name"s, "Adds author"s, std::bind(&View::AddAuthor, this, ph::_1)
        // либо
        // [this](auto& cmd_input) { return AddAuthor(cmd_input); }
    );

    menu_.AddAction(  //
        "ShowAuthors"s, "-"s, "Shows all authors"s, std::bind(&View::ShowAuthors, this, ph::_1)
    );

    menu_.AddAction(  //
        "AddBook"s, "pub_year, title"s, "Adds book"s, std::bind(&View::AddBook, this, ph::_1)
    );

    menu_.AddAction(  //
        "ShowBooks"s, "-"s, "Shows book"s, std::bind(&View::ShowBooks, this, ph::_1)
    );

    menu_.AddAction(  //
        "ShowAuthorBooks"s, "-"s, "Shows book of selected author"s, std::bind(&View::ShowAuthorBooks, this, ph::_1)
    );

    menu_.AddAction(  //
        "DeleteAuthor"s, "name"s, "Deletes author"s, std::bind(&View::DeleteAuthor, this, ph::_1)
    );

    menu_.AddAction(  //
        "EditAuthor"s, "name"s, "Updates author name"s, std::bind(&View::UpdateAuthor, this, ph::_1)
    );

    menu_.AddAction(  //
        "ShowBook"s, "name"s, "Shows book"s, std::bind(&View::ShowBook, this, ph::_1)
    );

    menu_.AddAction(  //
        "DeleteBook"s, "name"s, "Deletes book"s, std::bind(&View::DeleteBook, this, ph::_1)
    );
}

std::string View::GetTagsFromInput()
{
    output_ << "Enter tags (comma separated) or empty line to cancel:"sv << std::endl;
    std::string tags;
    std::getline(std::cin, tags);
    return tags;
}

void View::AddTags(const std::string& tags, const std::string& book_id)
{
    std::vector<std::string> tag_words;
    boost::split(tag_words, tags, boost::is_any_of(","), boost::token_compress_on);
    for (auto& word : tag_words)
    {
        boost::algorithm::trim(word);
    }
    std::sort(tag_words.begin(), tag_words.end());
    tag_words.erase(std::unique(tag_words.begin(), tag_words.end()), tag_words.end());

    for (auto& word : tag_words)
    {
        use_cases_.SaveTag(book_id, word);
    }
}

bool View::AddBookWithAuthorFromList(std::string& title, int year)
{
    output_ << "Select author:"sv << std::endl;
    auto authors = use_cases_.ShowAuthors();
    std::vector<std::string> authors_vec;

    for (int i = 1; auto & author : authors)
    {
        output_ << i << ". "sv << author.second << std::endl;
        authors_vec.push_back(author.first);
        ++i;
    }

    output_ << "Enter author # or empty line to cancel"sv << std::endl;
    int num = 0;
    std::cin >> num;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (num == 0)
    {
        return true;
    }
    else
    {
        std::string tags = GetTagsFromInput();
        auto book_id = use_cases_.AddBook(std::move(title), std::move(authors_vec[num - 1]), year);
        AddTags(tags, book_id);
        return true;
    }
}

bool View::AddBookWithAuthorFromString(std::string& title, std::string& author_name, int year)
{
    auto author_id = use_cases_.GetAuthorIdByName(author_name);
    if (author_id.empty())
    {
        output_ << "No author found. Do you want to add "sv << author_name << " (y/n)?"sv << std::endl;
        std::string answer;
        std::cin >> answer;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (answer == "Y"s || answer == "y"s)
        {
            std::string tags = GetTagsFromInput();

            auto author_id_new = use_cases_.AddAuthor(author_name);
            auto book_id = use_cases_.AddBook(title, author_id_new, year);

            AddTags(tags, book_id);
            return true;
        }
        else
        {
            output_ << "Failed to add book"sv << std::endl;
            return true;
        }
    }
    else
    {
        std::string tags = GetTagsFromInput();

        auto book_id = use_cases_.AddBook(title, author_id, year);

        AddTags(tags, book_id);
        return true;
    }
}

bool View::AddAuthor(std::istream& cmd_input) {
    try {
        std::string name;
        std::getline(cmd_input, name);
        boost::algorithm::trim(name);
        use_cases_.AddAuthor(std::move(name));
    } catch (const std::exception&) {
        output_ << "Failed to add author"sv << std::endl;
    }
    return true;
}

bool View::ShowAuthors(std::istream& cmd_input)
{
    try {
        auto authors = use_cases_.ShowAuthors();
        for (int i = 1; auto& author : authors)
        {
            output_ << i << " "sv << author.second << std::endl;
            ++i;
        }
    }
    catch (const std::exception& e) {        
        output_ << "Failed to show authors: "sv << e.what() << std::endl;
    }
    return true;
}

bool View::DeleteAuthor(std::istream& cmd_input)
{
    try {

        std::string author_name;
        std::getline(cmd_input, author_name);
        boost::algorithm::trim(author_name);

        if (author_name.empty())
        {
            auto authors = use_cases_.ShowAuthors();
            std::vector<std::string> authors_vec;

            for (int i = 1; auto & author : authors)
            {
                output_ << i << ". "sv << author.second << std::endl;
                authors_vec.push_back(author.first);
                ++i;
            }

            output_ << "Enter author # or empty line to cancel"sv << std::endl;
            int num = 0;
            std::cin >> num;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (num <= 0 || num > authors_vec.size())
            {
                return true;
            }
            else
            {
                use_cases_.DeleteAuthor(std::move(authors_vec[num - 1]));
                return true;
            }
        }
        else
        {
            auto author_id = use_cases_.GetAuthorIdByName(author_name);
            if (author_id.empty())
            {
                output_ << "Failed to delete author"sv << std::endl;
                return true;
            }
            else
            {
                use_cases_.DeleteAuthor(std::move(author_id));
                return true;
            }
        }
        
    }
    catch (const std::exception& e) {
        output_ << "Failed to delete author: "sv << e.what() << std::endl;
    }
    return true;
}

bool View::UpdateAuthor(std::istream& cmd_input)
{
    try {

        std::string author_name;
        std::getline(cmd_input, author_name);
        boost::algorithm::trim(author_name);

        if (author_name.empty())
        {
            auto authors = use_cases_.ShowAuthors();
            std::vector<std::string> authors_vec;

            for (int i = 1; auto & author : authors)
            {
                output_ << i << ". "sv << author.second << std::endl;
                authors_vec.push_back(author.first);
                ++i;
            }

            output_ << "Enter author # or empty line to cancel"sv << std::endl;
            int num = 0;
            std::cin >> num;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (num <= 0 || num > authors_vec.size())
            {
                return true;
            }
            else
            {
                output_ << "Enter new name:"sv << std::endl;
                std::string new_name;
                std::getline(std::cin, new_name);
                boost::algorithm::trim(new_name);
                use_cases_.UpdateAuthor(std::move(authors_vec[num - 1]), new_name);
                return true;
            }
        }
        else
        {
            auto author_id = use_cases_.GetAuthorIdByName(author_name);
            if (author_id.empty())
            {
                output_ << "Failed to edit author"sv << std::endl;
                return true;
            }
            else
            {
                output_ << "Enter new name:"sv << std::endl;
                std::string new_name;
                std::getline(std::cin, new_name);
                boost::algorithm::trim(new_name);
                use_cases_.UpdateAuthor(std::move(author_id), new_name);
                return true;
            }
        }

    }
    catch (const std::exception& e) {
        output_ << "Failed to delete author: "sv << e.what() << std::endl;
    }
    return true;
}

bool View::AddBook(std::istream& cmd_input) {
    try {      
        int year;
        cmd_input >> year;

        std::string title;
        std::getline(cmd_input, title);
        boost::algorithm::trim(title);

        output_ << "Enter author name or empty line to select from list:"sv << std::endl;
        std::string author_name;
        std::getline(std::cin, author_name);
        boost::algorithm::trim(author_name);

        if (author_name.empty())
        {
            return AddBookWithAuthorFromList(title, year);
        }
        else
        {
            return AddBookWithAuthorFromString(title, author_name, year);
        }
    }
    catch (const std::exception& e) {
        output_ << "Failed to add book: "sv << e.what() << std::endl;
    }
    return true;
}

bool View::ShowBooks(std::istream& cmd_input)
{
    try {
        auto books = use_cases_.ShowBooks();
        for (int i = 1; auto & [title, year, id] : books)
        {
            output_ << i << " "sv << title << ", " << year << std::endl;
            ++i;
        }
    }
    catch (const std::exception& e) {
        output_ << "Failed to show books: "sv << e.what() << std::endl;
    }
    return true;
}

bool View::ShowAuthorBooks(std::istream& cmd_input)
{
    try {        

        output_ << "Select author:"sv << std::endl;
        auto authors = use_cases_.ShowAuthors();
        std::vector<std::string> authors_vec;

        for (int i = 1; auto & author : authors)
        {
            output_ << i << ". "sv << author.second << std::endl;
            authors_vec.push_back(author.first);
            ++i;
        }

        output_ << "Enter author # or empty line to cancel"sv << std::endl;
        int num = 0;
        std::cin >> num;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (num <= 0 || num > authors_vec.size())
        {
            return true;
        }
        else
        {
            auto books = use_cases_.ShowAuthorBooks(std::move(authors_vec[num - 1]));
            for (int i = 1; auto & book : books)
            {
                output_ << i << ". "sv << book.first << ", " << book.second << std::endl;
                ++i;
            }
        }
    }
    catch (const std::exception& e) {
        output_ << "Failed to add book: "sv << e.what() << std::endl;
    }
    return true;
}

bool View::ShowBook(std::istream& cmd_input)
{
    try {

        std::string book_name;
        std::getline(cmd_input, book_name);
        boost::algorithm::trim(book_name);

        if (book_name.empty())
        {
            auto books = use_cases_.ShowBooks();
            std::vector<std::string> books_vec;

            for (int i = 1; auto & [title, year, id] : books)
            {
                output_ << i << " "sv << title << ", " << year << std::endl;
                books_vec.push_back(id);
                ++i;
            }

            output_ << "Enter the book # or empty line to cancel:"sv << std::endl;
            int num = 0;
            std::cin >> num;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (num <= 0 || num > books_vec.size())
            {
                return true;
            }
            else
            {
                auto book = use_cases_.ShowBook(std::move(books_vec[num - 1]));
                output_ << "Title: "sv << book[0] << std::endl;
                output_ << "Author: "sv << book[1] << std::endl;
                output_ << "Publication year: "sv << book[2] << std::endl;
                if (!book[3].empty())
                {
                    output_ << "Tags: "sv << book[3] << std::endl;
                }
                return true;
            }
        }
        else
        {
            auto books = use_cases_.GetBooksByTitle(book_name);
            std::vector<std::string> book;
            if (books.size() == 1)
            {
                auto& [title, year, id] = books[0];
                book = use_cases_.ShowBook(id);
            }
            else if(books.size() > 1)
            {
                for (int i = 1; auto& [title, year, id] : books)
                {
                    output_ << i << " "sv << title << ", " << year << std::endl;
                    ++i;
                }
                output_ << "Enter the book # or empty line to cancel:"sv << std::endl;
                int num = 0;
                std::cin >> num;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                auto& [title, year, id] = books[num - 1];
                book = use_cases_.ShowBook(id);
            }
            else
            {
                return true;
            }
            output_ << "Title: "sv << book[0] << std::endl;
            output_ << "Author: "sv << book[1] << std::endl;
            output_ << "Publication year: "sv << book[2] << std::endl;
            if (!book[3].empty())
            {
                output_ << "Tags: "sv << book[3] << std::endl;
            }
            return true;
        }

    }
    catch (const std::exception& e) {
        output_ << "Failed to show book: "sv << e.what() << std::endl;
    }
    return true;
}

bool View::DeleteBook(std::istream& cmd_input)
{
    try {

        std::string book_name;
        std::getline(cmd_input, book_name);
        boost::algorithm::trim(book_name);

        if (book_name.empty())
        {
            auto books = use_cases_.ShowBooks();
            std::vector<std::string> books_vec;

            for (int i = 1; auto & [title, year, id] : books)
            {
                output_ << i << " "sv << title << ", " << year << std::endl;
                books_vec.push_back(id);
                ++i;
            }

            output_ << "Enter the book # or empty line to cancel:"sv << std::endl;
            int num = 0;
            std::cin >> num;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (num <= 0 || num > books_vec.size())
            {
                return true;
            }
            else
            {
                use_cases_.DeleteBook(std::move(books_vec[num - 1]));
                return true;
            }
        }
        else
        {
            auto books = use_cases_.GetBooksByTitle(book_name);
            if (books.size() == 1)
            {
                auto& [title, year, id] = books[0];
                use_cases_.DeleteBook(id);
                return true;
            }
            else if (books.size() > 1)
            {
                for (int i = 1; auto & [title, year, id] : books)
                {
                    output_ << i << " "sv << title << ", " << year << std::endl;
                    ++i;
                }
                output_ << "Enter the book # or empty line to cancel:"sv << std::endl;
                int num = 0;
                std::cin >> num;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                auto& [title, year, id] = books[num - 1];
                use_cases_.DeleteBook(id);
                return true;
            }
            else
            {
                return true;
            }         

        }

    }
    catch (const std::exception& e) {
        output_ << "Failed to show book: "sv << e.what() << std::endl;
    }
    return true;
}

bool View::UpdateBook(std::istream& cmd_input)
{
    try {

        std::string book_name;
        std::getline(cmd_input, book_name);
        boost::algorithm::trim(book_name);

        if (book_name.empty())
        {
            auto books = use_cases_.ShowBooks();
            std::vector<std::string> books_vec;

            for (int i = 1; auto & [title, year, id] : books)
            {
                output_ << i << " "sv << title << ", " << year << std::endl;
                books_vec.push_back(id);
                ++i;
            }

            output_ << "Enter the book # or empty line to cancel:"sv << std::endl;
            int num = 0;
            std::cin >> num;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (num <= 0 || num > books_vec.size())
            {
                return true;
            }
            else
            {
                use_cases_.DeleteBook(std::move(books_vec[num - 1]));
                return true;
            }
        }
        else
        {
            auto books = use_cases_.GetBooksByTitle(book_name);
            if (books.size() == 1)
            {
                auto& [title, year, id] = books[0];
                use_cases_.DeleteBook(id);
                return true;
            }
            else if (books.size() > 1)
            {
                for (int i = 1; auto & [title, year, id] : books)
                {
                    output_ << i << " "sv << title << ", " << year << std::endl;
                    ++i;
                }
                output_ << "Enter the book # or empty line to cancel:"sv << std::endl;
                int num = 0;
                std::cin >> num;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                auto& [title, year, id] = books[num - 1];
                use_cases_.DeleteBook(id);
                return true;
            }
            else
            {
                return true;
            }

        }

    }
    catch (const std::exception& e) {
        output_ << "Failed to show book: "sv << e.what() << std::endl;
    }
    return true;
}

}  // namespace ui
