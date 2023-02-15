#include <catch2/catch_test_macros.hpp>

#include "../src/author.h"
#include "../src/book.h"
#include "../src/use_cases_impl.h"

namespace {

struct MockAuthorRepository : domain::AuthorRepository {
    std::vector<domain::Author> saved_authors;

    void Save(const domain::Author& author) override {
        saved_authors.emplace_back(author);
    }

    std::unordered_map<std::string, std::string> ShowAuthors() override {
        return std::unordered_map<std::string, std::string>();
    }
};

struct MockBookRepository : domain::BookRepository {
    std::vector<domain::Book> saved_books;

    void Save(const domain::Book& book)
    {
        saved_books.emplace_back(book);
    }
    std::vector<std::pair<std::string, std::string>> ShowBooks()
    {
        return std::vector<std::pair<std::string, std::string>>();
    }
    std::vector<std::pair<std::string, std::string>> ShowAuthorBooks(const std::string& id)
    {
        return std::vector<std::pair<std::string, std::string>>();
    }
};

struct Fixture {
    MockAuthorRepository authors;
    MockBookRepository books;
};

}  // namespace

SCENARIO_METHOD(Fixture, "Book Adding") {
    GIVEN("Use cases") {
        app::UseCasesImpl use_cases{authors, books};

        WHEN("Adding an author") {
            const auto author_name = "Joanne Rowling";
            use_cases.AddAuthor(author_name);

            THEN("author with the specified name is saved to repository") {
                REQUIRE(authors.saved_authors.size() == 1);
                CHECK(authors.saved_authors.at(0).GetName() == author_name);
                CHECK(authors.saved_authors.at(0).GetId() != domain::AuthorId{});
            }
        }
    }
}