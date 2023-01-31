#pragma once
#include <iosfwd>
#include <string>

namespace menu {
class Menu;
}

namespace app {
class UseCases;
}

namespace ui {

class View {
public:
    View(menu::Menu& menu, app::UseCases& use_cases, std::istream& input, std::ostream& output);

private:
    bool AddAuthor(std::istream& cmd_input);
    bool ShowAuthors(std::istream& cmd_input);
    bool DeleteAuthor(std::istream& cmd_input);
    bool UpdateAuthor(std::istream& cmd_input);

    bool AddBook(std::istream& cmd_input);
    bool ShowBooks(std::istream& cmd_input);
    bool ShowAuthorBooks(std::istream& cmd_input);
    bool ShowBook(std::istream& cmd_input);
    bool DeleteBook(std::istream& cmd_input);
    bool UpdateBook(std::istream& cmd_input);

    bool AddBookWithAuthorFromList(std::string& title, int year);
    bool AddBookWithAuthorFromString(std::string& title, std::string& author_name, int year);
    void AddTags(const std::string& tags, const std::string& book_id);
    std::string GetTagsFromInput();

    menu::Menu& menu_;
    app::UseCases& use_cases_;
    std::istream& input_;
    std::ostream& output_;
};

}  // namespace ui