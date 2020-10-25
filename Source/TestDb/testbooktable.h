#ifndef TESTBOOKTABLE_H
#define TESTBOOKTABLE_H

#include <QtTest>

#include "booktable.h"
#include "readsqlscript.h"


class TestBookTable : public QObject
{
    Q_OBJECT

public:
    TestBookTable();
    BookTable* booktable;

private Q_SLOTS:
    void initTestCase();

    void testGetBook();
    void testGetAllBooks();
    void testInsert();
    void testInsert2();
    void testRemove();

    void testSearchTitle();
    void testSearchSubTitle();
    void testSearchPublisher();
    void testSearchAuthor();
    void testSearchSubject();
    void testSearchComment();

    void testGetByGrade();
    void testGetByEdition();
    void testGetByStock();
    void testGetByAvailable();
    void testGetByStocktakingCount();
    void testGetByStocktakingDate();

    void testUpdateTitle();
    void testUpdateSubTitle();
    void testUpdatePublisher();
    void testUpdateAuthor();
    void testUpdateSubject();
    void testUpdateGrade();
    void testUpdateEdition();
    void testUpdatePrice();
    void testUpdateStock();
    void testUpdateAvailable();
    void testUpdateComment();
    void testUpdateStockTakingCount();
    void testUpdateStockTakingDate();

    void cleanupTestCase();
};

#endif // TESTBOOKTABLE_H
