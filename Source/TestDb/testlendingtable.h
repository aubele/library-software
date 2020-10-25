#ifndef TESTLENDINGTABLE_H
#define TESTLENDINGTABLE_H

#include <QTest>
#include "readsqlscript.h"
#include "lendingtable.h"
#include "givebacktable.h"


class TestLendingTable : public QObject
{
    Q_OBJECT
public:
    TestLendingTable();
    LendingTable* lendingtable;
    GiveBackTable* givebacktable;

private Q_SLOTS:
    void initTestCase();

    void testGetLending();
    void testGetGiveBack();
    void testGetLendingFromGiveBackId();
    void testGetLendingsFromBook();
    void testGetLendingsFromType();
    void testGetLendingsFromStudent();
    void testGetOpenLendingsFromStudent();
    void testGetLendingWithDamage();
    void testGetRowsByDate();
    void testSearchRowsByDamage();

    void testInsertLending();
    void testInsertLending2();
    void testInsertGiveBack();
    void testRemoveLending();
    void testUpdateGiveBackDamage();

    void cleanupTestCase();
};

#endif // TESTLENDINGTABLE_H
