#pragma once
#include "author_fwd.h"
#include "book_fwd.h"
#include "use_cases.h"

namespace app {

class UseCasesImpl : public UseCases {
public:
    explicit UseCasesImpl(domain::AuthorRepository& authors, domain::BookRepository& books)
        : authors_{ authors }
        , books_{ books } {
    }

    std::string AddAuthor(const std::string& name) override;

    std::vector<std::pair<std::string, std::string>> ShowAuthors() override;

    std::string GetAuthorIdByName(const std::string& name) override;

    void DeleteAuthor(const std::string& author_id) override;

    void UpdateAuthor(const std::string& author_id, const std::string& name) override;

    std::string AddBook(const std::string& title, const std::string& author_id, int publication_year) override;

    std::vector<std::tuple<std::string, std::string, std::string>> ShowBooks() override;

    std::vector<std::pair<std::string, std::string>> ShowAuthorBooks(const std::string& id) override;

    void SaveTag(const std::string& book_id, const std::string& tag) override;

    void DeleteAllBookTags(const std::string& book_id) override;

    void DeleteAuthorBooks(const std::string& author_id) override;

    std::vector<std::string> ShowBook(std::string book_id) override;

    std::vector<std::tuple<std::string, std::string, std::string>> GetBooksByTitle(const std::string& name) override;

    void DeleteBook(const std::string& book_id) override;

    void UpdateBook(const std::string& book_id, const std::string& title) override;

private:
    domain::AuthorRepository& authors_;

    domain::BookRepository& books_;
};

}  // namespace app
