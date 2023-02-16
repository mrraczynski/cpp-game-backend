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

    void AddAuthor(const std::string& name) override;

    std::vector<std::pair<std::string, std::string>> ShowAuthors() override;

    void AddBook(const std::string& title, const std::string& author_id, int publication_year) override;

    std::vector<std::pair<std::string, std::string>> ShowBooks() override;

    std::vector<std::pair<std::string, std::string>> ShowAuthorBooks(const std::string& id) override;

private:
    domain::AuthorRepository& authors_;

    domain::BookRepository& books_;
};

}  // namespace app
