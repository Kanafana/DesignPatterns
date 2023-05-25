#include "LibraryFacade.h"

LibraryFacade::LibraryFacade() {
    database = Database::getInstance();
    librarian = Librarian::getInstance();
    pool = new BookPool();
    moveStrategy = nullptr;
}

LibraryFacade::~LibraryFacade() {
    delete pool;
    if (moveStrategy) delete moveStrategy;
}

void LibraryFacade::createShelf(const std::string & shelfName) {
    Shelf shelf = ShelfBuilder(shelfName).build();
    database->addShelf(shelf);
    librarian->notify("Added new shelf: " + shelfName);
}

void LibraryFacade::displayShelves() const {
    database->displayShelves();
}

void LibraryFacade::displayShelves(const std::string& title) const {
    database->displayShelves(title);
}

void LibraryFacade::createBook(const std::string& title, const std::string& author, int pageCount) {
    Book book = BookFactory::createBook(title, author, pageCount);
    pool->returnBook(book);
    librarian->notify("Added new book: \"" + title + "\" by " + author);
}

void LibraryFacade::placeBook(const std::string& bookTitle, const std::string& shelfTitle) {
    Book bookToPlace = BookFactory::createBook("", "", -1);
    Shelf* destinationShelf = nullptr;

    for (auto& shelf : *database->getShelves()) {
        if (shelf.getTitle() == shelfTitle) {
            destinationShelf = &shelf;
            break;
        }
    }

    if (destinationShelf) {
        if (pool->contains(bookTitle)) {
            while (true) {
                bookToPlace = pool->getBook();
                if (bookToPlace.getTitle() != bookTitle) {
                    pool->returnBook(bookToPlace);
                }
                else break;
            }

            if (moveStrategy) delete moveStrategy;
            moveStrategy = new NewMoveStrategy();

            moveStrategy->moveBook(bookToPlace, *destinationShelf, *destinationShelf);
            librarian->notify("Book \"" + bookToPlace.getTitle() + "\" by " + bookToPlace.getAuthor() + " was placed on \"" + shelfTitle + "\" shelf");
        }
        else {
            throw "Book to move not found.";
        }
    }
    else {
        throw "Destination shelf not found.";
    }
}

void LibraryFacade::moveBook(const std::string& bookTitle, const std::string& sourceTitle, const std::string& destinationTitle) {
    Book bookToMove = BookFactory::createBook("", "", -1);
    Shelf* sourceShelf = nullptr;
    Shelf* destinationShelf = nullptr;

    for (auto& shelf : *database->getShelves()) {
        if (shelf.getTitle() == sourceTitle) {
            sourceShelf = &shelf;
        }
        if (shelf.getTitle() == destinationTitle) {
            destinationShelf = &shelf;
        }
        if (sourceShelf && destinationShelf) {
            break;
        }
    }

    if (sourceShelf && destinationShelf) {
        for (auto& book : *sourceShelf->getBooks()) {
            if (book.getTitle() == bookTitle) {
                bookToMove = book;
                break;
            }
        }

        if (bookToMove.getPageCount() != -1) {
            if (moveStrategy) delete moveStrategy;
            moveStrategy = new ExistingMoveStrategy();

            moveStrategy->moveBook(bookToMove, *sourceShelf, *destinationShelf);
            librarian->notify("Book \"" + bookToMove.getTitle() + "\" by " + bookToMove.getAuthor() + " was replaced on \"" + destinationTitle + "\" shelf");
        }
        else {
            throw "Book to move not found.";
        }
    }
    else {
        throw "Source shelf or destination shelf not found.";
    }
}

void LibraryFacade::createReader(const std::string& readerName) {
    LibraryReader reader = LibraryReaderFactory::createLibrarySubscriber(readerName);
    database->addReader(reader);
}

void LibraryFacade::displayReaders() const {
    database->displayReaders();
}

void LibraryFacade::displayReaders(const std::string& title) const {
    database->displayReaders(title);
}

void LibraryFacade::subscribe(const std::string& readerName) {
    LibraryReader* reader = nullptr;

    for (auto& subscriber : *database->getReaders()) {
        if (subscriber.getName() == readerName) {
            reader = &subscriber;
            break;
        }
    }

    if (reader) {
        librarian->subscribe(reader);
    }
    else {
        throw "Reader not found";
    }
}

void LibraryFacade::unsubscribe(const std::string& readerName) {
    LibraryReader* reader = nullptr;

    for (auto& subscriber : *database->getReaders()) {
        if (subscriber.getName() == readerName) {
            reader = &subscriber;
            break;
        }
    }

    if (reader) {
        librarian->unsubscribe(reader);
    }
    else {
        throw "Reader not found";
    }
}