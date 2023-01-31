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
    virtual std::vector<std::tuple<std::string, std::string, std::string>> ShowBooks() = 0;
    virtual std::vector<std::pair<std::string, std::string>> ShowAuthorBooks(const std::string& id) = 0;
    virtual void SaveTag(const std::string& book_id, const std::string& tag) = 0;
    virtual void DeleteAllBookTags(const std::string& book_id) = 0;
    virtual void DeleteAuthorBooks(const std::string& author_id) = 0;
    virtual std::vector<std::string> ShowBook(std::string book_id) = 0;
    virtual std::vector<std::tuple<std::string, std::string, std::string>> GetBooksByTitle(const std::string& name) = 0;
    virtual void DeleteBook(const std::string& book_id) = 0;
    virtual void UpdateBook(const std::string& book_id, const std::string& title) = 0;

protected:
    ~BookRepository() = default;
};

}  // namespace domain
