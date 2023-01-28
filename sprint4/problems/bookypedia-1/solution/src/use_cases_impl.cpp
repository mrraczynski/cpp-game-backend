#include "use_cases_impl.h"

#include "author.h"
#include "book.h"

namespace app {
using namespace domain;

void UseCasesImpl::AddAuthor(const std::string& name) {
    authors_.Save({AuthorId::New(), name});
}

std::unordered_map<std::string, std::string> UseCasesImpl::ShowAuthors() {
    return authors_.ShowAuthors();
}

void UseCasesImpl::AddBook(const std::string& title, const std::string& author_id, int publication_year) {
    books_.Save(Book(BookId::New(), author_id, title, publication_year));
}

std::vector<std::pair<std::string, std::string>> UseCasesImpl::ShowBooks()
{
    return books_.ShowBooks();
}

std::vector<std::pair<std::string, std::string>> UseCasesImpl::ShowAuthorBooks(const std::string& id)
{
    return books_.ShowAuthorBooks(id);
}

}  // namespace app
