#pragma once
#include <string>
#include <vector>

#include "tagged_uuid.h"
//#include "author.h"

namespace domain {

namespace detail {
struct BookTag {};
}  // namespace detail

using BookId = util::TaggedUUID<detail::BookTag>;

class Book {
public:
    Book(BookId id, std::string author_id, std::string title, int publication_year)
        : id_(std::move(id))
        , author_id_(std::move((author_id)))
        , title_(std::move(title))
        , publication_year_ (publication_year) {
    }

    const BookId& GetId() const noexcept {
        return id_;
    }

    const std::string& GetTitle() const noexcept {
        return title_;
    }

    const std::string& GetAuthorId() const noexcept {
        return author_id_;
    }

    int GetPublicationYear() const noexcept {
        return publication_year_;
    }

private:
    BookId id_;
    std::string author_id_;
    std::string title_;
    int publication_year_;
};

class BookRepository {
public:
    virtual void Save(const Book& book) = 0;
    virtual std::vector<std::pair<std::string, std::string>> ShowBooks() = 0;
    virtual std::vector<std::pair<std::string, std::string>> ShowAuthorBooks(const std::string& id) = 0;

protected:
    ~BookRepository() = default;
};

}  // namespace domain
