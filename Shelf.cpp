#include "Shelf.h"

std::string Shelf::getTitle() {
    return title;
}

std::vector<Book>* Shelf::getBooks() {
    return &books;
}

void Shelf::addBook(const Book& book) {
    books.push_back(book);
}

void Shelf::removeBook(const Book& book) {
    for (auto it = books.begin(); it != books.end(); ++it) {
        if (it->getTitle() == book.getTitle() && it->getAuthor() == book.getAuthor()) {
            books.erase(it);
            break;
        }
    }
}

bool Shelf::operator==(const Shelf& other) {
    return this->title == other.title;
}
