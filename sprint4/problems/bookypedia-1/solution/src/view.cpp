#include "view.h"

#include <boost/algorithm/string/trim.hpp>
#include <iostream>
#include <string>

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

bool View::AddBook(std::istream& cmd_input) {
    try {      
        int year;
        cmd_input >> year;

        std::string title;
        std::getline(cmd_input, title);
        boost::algorithm::trim(title);

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
            use_cases_.AddBook(std::move(title), std::move(authors_vec[num - 1]), year);
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
        auto authors = use_cases_.ShowBooks();
        for (int i = 1; auto& author : authors)
        {
            output_ << i << ". "sv << author.first << ", " << author.second << std::endl;
            ++i;
        }
    }
    catch (const std::exception& e) {
        output_ << "Failed to show authors: "sv << e.what() << std::endl;
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

        if (num == 0)
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

}  // namespace ui
