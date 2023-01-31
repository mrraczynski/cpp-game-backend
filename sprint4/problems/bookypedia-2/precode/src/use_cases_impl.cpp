#include "use_cases_impl.h"

#include "author.h"
#include "book.h"

namespace app {
using namespace domain;

std::string UseCasesImpl::AddAuthor(const std::string& name) {
    auto author_id = AuthorId::New();
    authors_.Save({ author_id, name});
    return author_id.ToString();
}

std::unordered_map<std::string, std::string> UseCasesImpl::ShowAuthors() {
    return authors_.ShowAuthors();
}

std::string UseCasesImpl::GetAuthorIdByName(const std::string& name)
{
    return authors_.GetAuthorIdByName(name);
}

void UseCasesImpl::DeleteAuthor(const std::string& author_id)
{
    authors_.DeleteAuthor(author_id);
}

void UseCasesImpl::UpdateAuthor(const std::string& author_id, const std::string& name)
{
    authors_.UpdateAuthor(author_id, name);
}

std::string UseCasesImpl::AddBook(const std::string& title, const std::string& author_id, int publication_year) {
    auto book_id = BookId::New();
    books_.Save(Book(book_id, author_id, title, publication_year));
    return book_id.ToString();
}

std::vector<std::tuple<std::string, std::string, std::string>> UseCasesImpl::ShowBooks()
{
    return books_.ShowBooks();
}

std::vector<std::pair<std::string, std::string>> UseCasesImpl::ShowAuthorBooks(const std::string& id)
{
    return books_.ShowAuthorBooks(id);
}

void UseCasesImpl::SaveTag(const std::string& book_id, const std::string& tag)
{
    books_.SaveTag(book_id, tag);
}

void UseCasesImpl::DeleteAllBookTags(const std::string& book_id)
{
    books_.DeleteAllBookTags(book_id);
}

void UseCasesImpl::DeleteAuthorBooks(const std::string& author_id)
{
    books_.DeleteAuthorBooks(author_id);
}

std::vector<std::string> UseCasesImpl::ShowBook(std::string book_id)
{
    return books_.ShowBook(book_id);
}

std::vector<std::tuple<std::string, std::string, std::string>> UseCasesImpl::GetBooksByTitle(const std::string& name)
{
    return books_.GetBooksByTitle(name);
}

void UseCasesImpl::DeleteBook(const std::string& book_id)
{
    books_.DeleteBook(book_id);
}

void UseCasesImpl::UpdateBook(const std::string& book_id, const std::string& title)
{
    books_.UpdateBook(book_id, title);
}

}  // namespace app
