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

        std::vector<std::pair<std::string, std::string>> ShowAuthors() override {
            return std::vector<std::pair<std::string, std::string>>();
        }

        std::string GetAuthorIdByName(const std::string& name) override {
            return std::string();
        }
        void DeleteAuthor(const std::string& author_id) override {
            return;
        }
        void UpdateAuthor(const std::string& author_id, const std::string& name) override {
            return;
        }
    };

    struct MockBookRepository : domain::BookRepository {
        std::vector<domain::Book> saved_books;

        void Save(const domain::Book& book) override
        {
            saved_books.emplace_back(book);
        }
        std::vector<std::tuple<std::string, std::string, std::string>> ShowBooks() override
        {
            return std::vector<std::tuple<std::string, std::string, std::string>>();
        }
        std::vector<std::pair<std::string, std::string>> ShowAuthorBooks(const std::string& id) override
        {
            return std::vector<std::pair<std::string, std::string>>();
        }

        void SaveTag(const std::string& book_id, const std::string& tag) override {
            return;
        }
        void DeleteAllBookTags(const std::string& book_id) override {
            return;
        }
        void DeleteAuthorBooks(const std::string& author_id) override {
            return;
        }
        std::vector<std::string> ShowBook(std::string book_id) override {
            return std::vector<std::string>();
        }
        std::vector<std::tuple<std::string, std::string, std::string>> GetBooksByTitle(const std::string& name) override {
            return std::vector<std::tuple<std::string, std::string, std::string>>();
        }
        void DeleteBook(const std::string& book_id) override {
            return;
        }
        void UpdateBook(const std::string& book_id, const std::string& title) override {
            return;
        }
    };

    struct Fixture {
        MockAuthorRepository authors;
        MockBookRepository books;
    };

}  // namespace

SCENARIO_METHOD(Fixture, "Book Adding") {
    GIVEN("Use cases") {
        app::UseCasesImpl use_cases{ authors, books };

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