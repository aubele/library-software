#include "testbooktable.h"

//--------------------
// Constructor
//--------------------
TestBookTable::TestBookTable()
{
    booktable = new BookTable("BookTable", "isbn", "title", "subtitle", "publisher", "author",
                              "subject", "grade", "edition", "price", "stock", "available",
                              "comment", "stocktakingCount", "stocktakingDate");
}

//--------------------
// initTestCase: initiates the testdatabase via sql-scriptfile
//--------------------
void TestBookTable::initTestCase()
{
    // Create test db
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("../TestDb/TestDatabase.db");
    db.open();

    // Write data from script file into db
    // Wrong queries get displayed via qDebug
    int queryCount = ReadSqlScriptFile(db, "../TestDb/db_with_testdata.db.sql");
    qDebug() << "Successful queries:" << queryCount;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------
// testGetBook: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testGetBook()
{
    // Existing
    BookTable::BookRow* row = booktable->getBook("975-1");
    QVERIFY(row->isbn == "975-1");
    QVERIFY(row->title == "Buch1");
    QVERIFY(row->subtitle == "Unter1");
    QVERIFY(row->publisher == "Hans1");
    QVERIFY(row->author == "B1");
    QVERIFY(row->subject == "Fach1");
    QVERIFY(row->grade.at(0) == 5);
    QVERIFY(row->grade.at(1) == 6);
    QVERIFY(row->edition == 1);
    QVERIFY(row->price == "1.00 €");
    QVERIFY(row->stock == 11);
    QVERIFY(row->available == 11);
    QVERIFY(row->comment == "Kommentar1");
    QVERIFY(row->stocktakingCount == 1);
    QVERIFY(row->stocktakingDate == "1.1.2000");

    // Only one grade
    row = booktable->getBook("975-2");
    QVERIFY(row->grade.at(0) == 5);

    // Not existing
    row = booktable->getBook("666-6");
    QVERIFY(row->isbn.isEmpty());
}

//--------------------
// testInsert: tests for all books
//--------------------
void TestBookTable::testGetAllBooks()
{
    QList<BookTable::BookRow> rowList = booktable->getAllBooks();
    QVERIFY(rowList.size() == 10);
}

//--------------------
// testInsert: tests for a new book and an already existing book
//--------------------
void TestBookTable::testInsert()
{
    // New Entry
    QList<int> grades = {9,10};
    QVERIFY(booktable->insert("111-1", "Biologie Now", "Extra", "Timm", "Herku Ma", "Biologie", grades, 4, "9.99 €", 60, 55, "Neu", 60, "20.2.2015"));
    testGetBook();
    BookTable::BookRow* row = booktable->getBook("111-1");
    QVERIFY(row->isbn == "111-1");
    QVERIFY(row->title == "Biologie Now");
    QVERIFY(row->subtitle == "Extra");
    QVERIFY(row->publisher == "Timm");
    QVERIFY(row->author == "Herku Ma");
    QVERIFY(row->subject == "Biologie");
    QVERIFY(row->grade.at(0) == 9);
    QVERIFY(row->grade.at(1) == 10);
    QVERIFY(row->edition == 4);
    QVERIFY(row->price == "9.99 €");
    QVERIFY(row->stock == 60);
    QVERIFY(row->available == 55);
    QVERIFY(row->comment == "Neu");
    QVERIFY(row->stocktakingCount == 60);
    QVERIFY(row->stocktakingDate == "20.2.2015");

    // Already existing entry
    QVERIFY(booktable->insert("111-1", "Biologie Now", "Extra", "Timm", "Herku Ma", "Biologie", grades, 4, "9,99 €", 60, 55, "Neu", 60, "20.2.2015") == false);
}

//--------------------
// testInsert2: tests for a new book and an already existing book
//--------------------
void TestBookTable::testInsert2()
{
    // New Entry
    QList<int> grades = {8};
    QVERIFY(booktable->insert(BookTable::BookRow{"111-2", "Biologie Later", "Extra2", "Timm2", "Herku Ma2", "Biologie2", grades, 5, "15.99 €", 51, 9, "NeuNeu", 80, "27.6.2018"}));
    testGetBook();
    BookTable::BookRow* row = booktable->getBook("111-2");
    QVERIFY(row->isbn == "111-2");
    QVERIFY(row->title == "Biologie Later");
    QVERIFY(row->subtitle == "Extra2");
    QVERIFY(row->publisher == "Timm2");
    QVERIFY(row->author == "Herku Ma2");
    QVERIFY(row->subject == "Biologie2");
    QVERIFY(row->grade.at(0) == 8);
    QVERIFY(row->edition == 5);
    QVERIFY(row->price == "15.99 €");
    QVERIFY(row->stock == 51);
    QVERIFY(row->available == 9);
    QVERIFY(row->comment == "NeuNeu");
    QVERIFY(row->stocktakingCount == 80);
    QVERIFY(row->stocktakingDate == "27.6.2018");

    // Already existing entry
    QVERIFY(booktable->insert(BookTable::BookRow{"111-2", "Biologie Later", "Extra2", "Timm2", "Herku Ma2", "Biologie2", grades, 5, "15.99 €", 51, 9, "NeuNeu", 80, "27.6.2018"}) == false);
}

//--------------------
// testRemove: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testRemove()
{
    // Existing
    QVERIFY(booktable->remove("975-9"));
    testGetBook();
    BookTable::BookRow* row = booktable->getBook("975-9");
    QVERIFY(row->isbn.isEmpty());

    // Not existing
    QVERIFY(booktable->remove("666-6"));
    row = booktable->getBook("666-6");
    QVERIFY(row->isbn.isEmpty());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------
// testSearchTitle: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testSearchTitle()
{
    // Existing
    QList<BookTable::BookRow> rowList = booktable->searchBookByTitle("Buch1");
    QVERIFY(rowList.size() == 2);
    QVERIFY(rowList.at(0).isbn == "975-1");
    QVERIFY(rowList.at(1).isbn == "975-10");

    // Not Existing
    rowList = booktable->searchBookByTitle("Nicht");
    QVERIFY(rowList.size() == 0);
}

//--------------------
// testSearchSubTitle: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testSearchSubTitle()
{
    // Existing
    QList<BookTable::BookRow> rowList = booktable->searchBookBySubTitle("Unter1");
    QVERIFY(rowList.size() == 2);
    QVERIFY(rowList.at(0).isbn == "975-1");
    QVERIFY(rowList.at(1).isbn == "975-10");

    // Not Existing
    rowList = booktable->searchBookBySubTitle("Nicht");
    QVERIFY(rowList.size() == 0);
}

//--------------------
// testSearchPublisher: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testSearchPublisher()
{
    // Existing
    QList<BookTable::BookRow> rowList = booktable->searchBookByPublisher("Hans1");
    QVERIFY(rowList.size() == 2);
    QVERIFY(rowList.at(0).isbn == "975-1");
    QVERIFY(rowList.at(1).isbn == "975-10");

    // Not Existing
    rowList = booktable->searchBookByPublisher("Nicht");
    QVERIFY(rowList.size() == 0);
}

//--------------------
// testSearchAuthor: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testSearchAuthor()
{
    // Existing
    QList<BookTable::BookRow> rowList = booktable->searchBookByAuthor("B1");
    QVERIFY(rowList.size() == 2);
    QVERIFY(rowList.at(0).isbn == "975-1");
    QVERIFY(rowList.at(1).isbn == "975-10");

    // Not Existing
    rowList = booktable->searchBookByAuthor("Nicht");
    QVERIFY(rowList.size() == 0);
}

//--------------------
// testSearchSubject: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testSearchSubject()
{
    // Existing
    QList<BookTable::BookRow> rowList = booktable->searchBooksBySubject("Fach1");
    QVERIFY(rowList.size() == 2);
    QVERIFY(rowList.at(0).isbn == "975-1");
    QVERIFY(rowList.at(1).isbn == "975-10");

    // Not Existing
    rowList = booktable->searchBooksBySubject("Nicht");
    QVERIFY(rowList.size() == 0);
}

//--------------------
// testSearchComment: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testSearchComment()
{
    // Existing
    QList<BookTable::BookRow> rowList = booktable->searchBooksByComment("Kommentar1");
    QVERIFY(rowList.size() == 2);
    QVERIFY(rowList.at(0).isbn == "975-1");
    QVERIFY(rowList.at(1).isbn == "975-10");

    // Not Existing
    rowList = booktable->searchBooksByComment("Nicht");
    QVERIFY(rowList.size() == 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------
// testGetByGrade: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testGetByGrade()
{
    // Existing
    QList<BookTable::BookRow> rowList = booktable->getBooksByGrade(5);
    QVERIFY(rowList.size() == 4);
    QVERIFY(rowList.at(0).isbn == "975-1");
    QVERIFY(rowList.at(1).isbn == "975-2");
    QVERIFY(rowList.at(2).isbn == "975-3");
    QVERIFY(rowList.at(3).isbn == "975-4");

    // Not Existing
    rowList = booktable->getBooksByGrade(18);
    QVERIFY(rowList.size() == 0);
}

//--------------------
// testGetByEdition: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testGetByEdition()
{
    // Existing
    QList<BookTable::BookRow> rowList = booktable->getBooksByEdition(1);
    QVERIFY(rowList.size() == 2);
    QVERIFY(rowList.at(0).isbn == "975-1");
    QVERIFY(rowList.at(1).isbn == "975-2");

    // Not Existing
    rowList = booktable->getBooksByEdition(99);
    QVERIFY(rowList.size() == 0);
}

//--------------------
// testGetByStock: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testGetByStock()
{
    // Existing
    QList<BookTable::BookRow> rowList = booktable->getBooksByStock(11);
    QVERIFY(rowList.size() == 2);
    QVERIFY(rowList.at(0).isbn == "975-1");
    QVERIFY(rowList.at(1).isbn == "975-2");

    // Not Existing
    rowList = booktable->getBooksByEdition(0);
    QVERIFY(rowList.size() == 0);
}

//--------------------
// testGetByAvailable: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testGetByAvailable()
{
    // Existing
    QList<BookTable::BookRow> rowList = booktable->getBooksByAvailable(11);
    QVERIFY(rowList.size() == 2);
    QVERIFY(rowList.at(0).isbn == "975-1");
    QVERIFY(rowList.at(1).isbn == "975-2");

    // Not Existing
    rowList = booktable->getBooksByAvailable(0);
    QVERIFY(rowList.size() == 0);
}

//--------------------
// testGetByStocktakingCount: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testGetByStocktakingCount()
{
    // Existing
    QList<BookTable::BookRow> rowList = booktable->getBooksByStocktakingCount(1);
    QVERIFY(rowList.size() == 2);
    QVERIFY(rowList.at(0).isbn == "975-1");
    QVERIFY(rowList.at(1).isbn == "975-2");

    // Not Existing
    rowList = booktable->getBooksByStocktakingCount(999);
    QVERIFY(rowList.size() == 0);
}

//--------------------
// testGetByStocktakingDate: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testGetByStocktakingDate()
{
    // Existing
    QList<BookTable::BookRow> rowList = booktable->getBooksByStocktakingDate("1.1.2000");
    QVERIFY(rowList.size() == 2);
    QVERIFY(rowList.at(0).isbn == "975-1");
    QVERIFY(rowList.at(1).isbn == "975-2");

    // Not Existing
    rowList = booktable->getBooksByStocktakingDate("1.1.2999");
    QVERIFY(rowList.size() == 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------
// testUpdateTitle: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testUpdateTitle()
{
    // Existing
    QVERIFY(booktable->updateTitle("975-7", "NeuerTitel"));
    testGetBook();
    BookTable::BookRow* row = booktable->getBook("975-7");
    QVERIFY(row->isbn == "975-7");
    QVERIFY(row->title == "NeuerTitel");

    // Not Existing
    QVERIFY(booktable->updateTitle("666-6", "NeuerTitel"));
    row = booktable->getBook("666-6");
    QVERIFY(row->isbn.isEmpty());
    QVERIFY(row->title.isEmpty());
}

//--------------------
// testUpdateSubTitle: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testUpdateSubTitle()
{
    // Existing
    QVERIFY(booktable->updateSubTitle("975-7", "NeuerSubTitel"));
    testGetBook();
    BookTable::BookRow* row = booktable->getBook("975-7");
    QVERIFY(row->isbn == "975-7");
    QVERIFY(row->subtitle == "NeuerSubTitel");

    // Not Existing
    QVERIFY(booktable->updateSubTitle("666-6", "NeuerTitel"));
    row = booktable->getBook("666-6");
    QVERIFY(row->isbn.isEmpty());
    QVERIFY(row->subtitle.isEmpty());
}

//--------------------
// testUpdatePublisher: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testUpdatePublisher()
{
    // Existing
    QVERIFY(booktable->updatePublisher("975-7", "NeuerPub"));
    testGetBook();
    BookTable::BookRow* row = booktable->getBook("975-7");
    QVERIFY(row->isbn == "975-7");
    QVERIFY(row->publisher == "NeuerPub");

    // Not Existing
    QVERIFY(booktable->updatePublisher("666-6", "NeuerPub"));
    row = booktable->getBook("666-6");
    QVERIFY(row->isbn.isEmpty());
    QVERIFY(row->publisher.isEmpty());
}

//--------------------
// testUpdateAuthor: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testUpdateAuthor()
{
    // Existing
    QVERIFY(booktable->updateAuthor("975-7", "NeuerAuthor"));
    testGetBook();
    BookTable::BookRow* row = booktable->getBook("975-7");
    QVERIFY(row->isbn == "975-7");
    QVERIFY(row->author == "NeuerAuthor");

    // Not Existing
    QVERIFY(booktable->updateAuthor("666-6", "NeuerAuthor"));
    row = booktable->getBook("666-6");
    QVERIFY(row->isbn.isEmpty());
    QVERIFY(row->author.isEmpty());
}

//--------------------
// testUpdateSubject: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testUpdateSubject()
{
    // Existing
    QVERIFY(booktable->updateSubject("975-7", "NeuesSubject"));
    testGetBook();
    BookTable::BookRow* row = booktable->getBook("975-7");
    QVERIFY(row->isbn == "975-7");
    QVERIFY(row->subject == "NeuesSubject");

    // Not Existing
    QVERIFY(booktable->updateSubject("666-6", "NeuesSubject"));
    row = booktable->getBook("666-6");
    QVERIFY(row->isbn.isEmpty());
    QVERIFY(row->subject.isEmpty());
}

//--------------------
// testUpdateGrade: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testUpdateGrade()
{
    // Existing
    QList<int> grades = {4,5};
    QVERIFY(booktable->updateGrade("975-7", grades));
    testGetBook();
    BookTable::BookRow* row = booktable->getBook("975-7");
    QVERIFY(row->isbn == "975-7");
    QVERIFY(row->grade.at(0) == 4);
    QVERIFY(row->grade.at(1) == 5);

    // Only one grade
    grades = {1};
    QVERIFY(booktable->updateGrade("975-7", grades));
    row = booktable->getBook("975-7");
    QVERIFY(row->isbn == "975-7");
    QVERIFY(row->grade.at(0) == 1);

    // Not Existing
    QVERIFY(booktable->updateGrade("666-6", grades));
    row = booktable->getBook("666-6");
    QVERIFY(row->isbn.isEmpty());
    QVERIFY(row->grade.isEmpty());
}

//--------------------
// testUpdateEdition: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testUpdateEdition()
{
    // Existing
    QVERIFY(booktable->updateEdition("975-7", 50));
    testGetBook();
    BookTable::BookRow* row = booktable->getBook("975-7");
    QVERIFY(row->isbn == "975-7");
    QVERIFY(row->edition == 50);

    // Not Existing
    QVERIFY(booktable->updateEdition("666-6", 50));
    row = booktable->getBook("666-6");
    QVERIFY(row->isbn.isEmpty());
    QVERIFY(row->edition == 0);
}

//--------------------
// testUpdatePrice: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testUpdatePrice()
{
    // Existing
    QVERIFY(booktable->updatePrice("975-7", "99.99 €"));
    testGetBook();
    BookTable::BookRow* row = booktable->getBook("975-7");
    QVERIFY(row->isbn == "975-7");
    QVERIFY(row->price == "99.99 €");

    // Not Existing
    QVERIFY(booktable->updatePrice("666-6", "99.99 €"));
    row = booktable->getBook("666-6");
    QVERIFY(row->isbn.isEmpty());
    QVERIFY(row->price.isEmpty());
}

//--------------------
// testUpdateStock: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testUpdateStock()
{
    // Existing
    QVERIFY(booktable->updateStock("975-7", 777));
    testGetBook();
    BookTable::BookRow* row = booktable->getBook("975-7");
    QVERIFY(row->isbn == "975-7");
    QVERIFY(row->stock == 777);

    // Not Existing
    QVERIFY(booktable->updateStock("666-6", 777));
    row = booktable->getBook("666-6");
    QVERIFY(row->isbn.isEmpty());
    QVERIFY(row->stock == 0);
}

//--------------------
// testUpdateAvailable: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testUpdateAvailable()
{
    // Existing
    QVERIFY(booktable->updateAvailable("975-7", 776));
    testGetBook();
    BookTable::BookRow* row = booktable->getBook("975-7");
    QVERIFY(row->isbn == "975-7");
    QVERIFY(row->available == 776);

    // Not Existing
    QVERIFY(booktable->updateAvailable("666-6", 776));
    row = booktable->getBook("666-6");
    QVERIFY(row->isbn.isEmpty());
    QVERIFY(row->available == 0);
}

//--------------------
// testUpdateComment: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testUpdateComment()
{
    // Existing
    QVERIFY(booktable->updateComment("975-7", "Ganz neuer Kommentar"));
    testGetBook();
    BookTable::BookRow* row = booktable->getBook("975-7");
    QVERIFY(row->isbn == "975-7");
    QVERIFY(row->comment == "Ganz neuer Kommentar");

    // Not Existing
    QVERIFY(booktable->updateComment("666-6", "Ganz neuer Kommentar"));
    row = booktable->getBook("666-6");
    QVERIFY(row->isbn.isEmpty());
    QVERIFY(row->comment.isEmpty());
}

//--------------------
// testUpdateStockTakingCount: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testUpdateStockTakingCount()
{
    // Existing
    QVERIFY(booktable->updateStocktakingCount("975-7", 90));
    testGetBook();
    BookTable::BookRow* row = booktable->getBook("975-7");
    QVERIFY(row->isbn == "975-7");
    QVERIFY(row->stocktakingCount == 90);

    // Not Existing
    QVERIFY(booktable->updateStocktakingCount("666-6", 90));
    row = booktable->getBook("666-6");
    QVERIFY(row->isbn.isEmpty());
    QVERIFY(row->stocktakingCount == 0);
}

//--------------------
// testUpdateStockTakingDate: tests for an existing book and a not existing book
//--------------------
void TestBookTable::testUpdateStockTakingDate()
{
    // Existing
    QVERIFY(booktable->updateStocktakingDate("975-7", "12.07.2006"));
    testGetBook();
    BookTable::BookRow* row = booktable->getBook("975-7");
    QVERIFY(row->isbn == "975-7");
    QVERIFY(row->stocktakingDate == "12.07.2006");

    // Not Existing
    QVERIFY(booktable->updateStocktakingDate("666-6", "12.07.2006"));
    row = booktable->getBook("666-6");
    QVERIFY(row->isbn.isEmpty());
    QVERIFY(row->stocktakingDate.isEmpty());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------
// cleanupTestCase: clean up and close database
//--------------------
void TestBookTable::cleanupTestCase()
{
    // Save the name in a String, else a warning gets thrown
    QString defaultConnectionName = QSqlDatabase::database().connectionName();
    QSqlDatabase::removeDatabase(defaultConnectionName);
    delete booktable;
}
