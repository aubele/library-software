#include "testlendingtable.h"

#include "QDebug"


//--------------------
// Constructor
//--------------------
TestLendingTable::TestLendingTable()
{
    givebacktable = new GiveBackTable("ReturnTable", "ID", "damage", "date");
    lendingtable = new LendingTable(*givebacktable, "LendingTable", "ID", "returnID", "lendDate", "book", "studentSurName",
                                    "studentPreName", "StudentDateOfBirth", "bringBackDate", "lendingType");
}

//--------------------
// initTestCase: initiates the testdatabase via sql-scriptfile
//--------------------
void TestLendingTable::initTestCase()
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------
// testGetLending: tests for an existing lending without giveback, with giveback and a not existing lending
//--------------------
void TestLendingTable::testGetLending()
{
    // Existing without giveback
    LendingTable::LendingRow* row = lendingtable->getLending(1);
    QVERIFY(row->ID == 1);
    QVERIFY(row->StudentForeName == "Vorname1-5a");
    QVERIFY(row->StudentSurName == "Nachname1-5a");
    QVERIFY(row->StudentBirthday == "31.07.2007");
    QVERIFY(row->BookIsbn == "975-1");
    QVERIFY(row->type == LendingTable::NORMAL);
    QVERIFY(row->LendDate == "12.09.2017");
    QVERIFY(row->BringBackDate == "20.08.2018");
    QVERIFY(row->GiveBackRow.ID == 0);
    QVERIFY(row->GiveBackRow.Damage == "");
    QVERIFY(row->GiveBackRow.Date == "");

    // Existing with giveback
    row = lendingtable->getLending(2);
    QVERIFY(row->ID == 2);
    QVERIFY(row->StudentForeName == "Vorname1-5a");
    QVERIFY(row->StudentSurName == "Nachname1-5a");
    QVERIFY(row->StudentBirthday == "31.07.2007");
    QVERIFY(row->BookIsbn == "975-2");
    QVERIFY(row->type == LendingTable::HOLIDAY);
    QVERIFY(row->LendDate == "12.09.2017");
    QVERIFY(row->BringBackDate == "20.10.2017");
    QVERIFY(row->GiveBackRow.ID == 1);
    QVERIFY(row->GiveBackRow.Damage == "");
    QVERIFY(row->GiveBackRow.Date == "20.10.2017");

    // Not Existing
    row = lendingtable->getLending(99);
    QVERIFY(row->ID == 0);
    QVERIFY(row->GiveBackRow.ID == 0);
}

//--------------------
// testGetGiveBack: tests for an existing giveback and a not existing giveback
//--------------------
void TestLendingTable::testGetGiveBack()
{
    // Existing
    GiveBackTable::GiveBackRow* row = givebacktable->getRow(2);
    QVERIFY(row->ID == 2);
    QVERIFY(row->Damage == "Wasserschaden");
    QVERIFY(row->Date == "20.10.2017");

    // Not Existing
    row = givebacktable->getRow(99);
    QVERIFY(row->ID == 0);
    QVERIFY(row->Damage.isEmpty());
    QVERIFY(row->Date.isEmpty());
}

//--------------------
// testGetLendingFromGiveBackId: tests for an existing lending without giveback, with giveback and a not existing lending
//--------------------
void TestLendingTable::testGetLendingFromGiveBackId()
{
    // Existing without giveback
    LendingTable::LendingRow* row = lendingtable->getLendingFromGiveBackId(0);
    QVERIFY(row->ID == 0);
    QVERIFY(row->GiveBackRow.ID == 0);

    // Existing with giveback
    row = lendingtable->getLendingFromGiveBackId(1);
    QVERIFY(row->ID == 2);
    QVERIFY(row->StudentForeName == "Vorname1-5a");
    QVERIFY(row->StudentSurName == "Nachname1-5a");
    QVERIFY(row->StudentBirthday == "31.07.2007");
    QVERIFY(row->BookIsbn == "975-2");
    QVERIFY(row->type == LendingTable::HOLIDAY);
    QVERIFY(row->LendDate == "12.09.2017");
    QVERIFY(row->BringBackDate == "20.10.2017");
    QVERIFY(row->GiveBackRow.ID == 1);
    QVERIFY(row->GiveBackRow.Damage == "");
    QVERIFY(row->GiveBackRow.Date == "20.10.2017");

    // Not Existing
    row = lendingtable->getLendingFromGiveBackId(99);
    QVERIFY(row->ID == 0);
    QVERIFY(row->GiveBackRow.ID == 0);
}

//--------------------
// testGetLendingsFromBook: tests for an existing lending and a not existing lending
//--------------------
void TestLendingTable::testGetLendingsFromBook()
{
    // Existing
    QList<LendingTable::LendingRow> rowList = lendingtable->getLendingsFromBook("975-1");
    QVERIFY(rowList.size() == 2);
    QVERIFY(rowList.at(0).ID == 1);
    QVERIFY(rowList.at(0).BookIsbn == "975-1");
    QVERIFY(rowList.at(0).GiveBackRow.ID == 0);
    QVERIFY(rowList.at(0).GiveBackRow.Damage == "");
    QVERIFY(rowList.at(0).GiveBackRow.Date == "");
    QVERIFY(rowList.at(1).ID == 4);
    QVERIFY(rowList.at(1).BookIsbn == "975-1");
    QVERIFY(rowList.at(1).GiveBackRow.ID == 2);
    QVERIFY(rowList.at(1).GiveBackRow.Damage == "Wasserschaden");
    QVERIFY(rowList.at(1).GiveBackRow.Date == "20.10.2017");

    // Not Existing
    rowList = lendingtable->getLendingsFromBook("900-99");
    QVERIFY(rowList.size() == 0);
}

//--------------------
// testGetLendingsFromType: tests for an existing lending
//--------------------
void TestLendingTable::testGetLendingsFromType()
{
    // Existing
    QList<LendingTable::LendingRow> rowList = lendingtable->getLendingsFromType(LendingTable::NORMAL);
    QVERIFY(rowList.size() == 3);
    QVERIFY(rowList.at(0).ID == 1);
    QVERIFY(rowList.at(0).type == LendingTable::NORMAL);
    QVERIFY(rowList.at(0).GiveBackRow.ID == 0);
    QVERIFY(rowList.at(0).GiveBackRow.Damage == "");
    QVERIFY(rowList.at(0).GiveBackRow.Date == "");
    QVERIFY(rowList.at(1).ID == 3);
    QVERIFY(rowList.at(1).type == LendingTable::NORMAL);
    QVERIFY(rowList.at(1).GiveBackRow.ID == 0);
    QVERIFY(rowList.at(1).GiveBackRow.Damage == "");
    QVERIFY(rowList.at(1).GiveBackRow.Date == "");
    QVERIFY(rowList.at(2).ID == 5);
    QVERIFY(rowList.at(2).type == LendingTable::NORMAL);
    QVERIFY(rowList.at(2).GiveBackRow.ID == 3);
    QVERIFY(rowList.at(2).GiveBackRow.Damage == "Zerknittert");
    QVERIFY(rowList.at(2).GiveBackRow.Date == "20.10.2017");
}

//--------------------
// testGetLendingsFromStudent: tests for an existing lending and a not existing lending
//--------------------
void TestLendingTable::testGetLendingsFromStudent()
{
    // Existing
    QList<LendingTable::LendingRow> rowList = lendingtable->getLendingsFromStudent("Vorname1-5a", "Nachname1-5a", "31.07.2007");
    QVERIFY(rowList.size() == 2);
    QVERIFY(rowList.at(0).ID == 1);
    QVERIFY(rowList.at(0).StudentForeName == "Vorname1-5a");
    QVERIFY(rowList.at(0).StudentSurName == "Nachname1-5a");
    QVERIFY(rowList.at(0).StudentBirthday == "31.07.2007");
    QVERIFY(rowList.at(0).GiveBackRow.ID == 0);
    QVERIFY(rowList.at(0).GiveBackRow.Damage == "");
    QVERIFY(rowList.at(0).GiveBackRow.Date == "");
    QVERIFY(rowList.at(1).ID == 2);
    QVERIFY(rowList.at(1).StudentForeName == "Vorname1-5a");
    QVERIFY(rowList.at(1).StudentSurName == "Nachname1-5a");
    QVERIFY(rowList.at(1).StudentBirthday == "31.07.2007");
    QVERIFY(rowList.at(1).GiveBackRow.ID == 1);
    QVERIFY(rowList.at(1).GiveBackRow.Damage == "");
    QVERIFY(rowList.at(1).GiveBackRow.Date == "20.10.2017");

    // Not Existing
    rowList = lendingtable->getLendingsFromStudent("Nicht", "Nicht", "Nicht");
    QVERIFY(rowList.size() == 0);
}

//--------------------
// testGetOpenLendingsFromStudent: tests for an existing lending with giveback, without giveback and a not existing lending
//--------------------
void TestLendingTable::testGetOpenLendingsFromStudent()
{
    // Existing + Should only return the lending with an empty giveback
    QList<LendingTable::LendingRow> rowList = lendingtable->getOpenLendingsFromStudent("Vorname1-5a", "Nachname1-5a", "31.07.2007");
    QVERIFY(rowList.size() == 1);
    QVERIFY(rowList.at(0).ID == 1);
    QVERIFY(rowList.at(0).StudentForeName == "Vorname1-5a");
    QVERIFY(rowList.at(0).StudentSurName == "Nachname1-5a");
    QVERIFY(rowList.at(0).StudentBirthday == "31.07.2007");
    QVERIFY(rowList.at(0).GiveBackRow.ID == 0);
    QVERIFY(rowList.at(0).GiveBackRow.Damage == "");
    QVERIFY(rowList.at(0).GiveBackRow.Date == "");

    // Not Existing
    rowList = lendingtable->getOpenLendingsFromStudent("Nicht", "Nicht", "Nicht");
    QVERIFY(rowList.size() == 0);
}

//--------------------
// testGetLendingWithDamage: tests for all lendings with damage
//--------------------
void TestLendingTable::testGetLendingWithDamage()
{
    // All lendings
    QList<LendingTable::LendingRow> rowList = lendingtable->getLendingsWithDamage();
    QVERIFY(rowList.size() == 3);
    QVERIFY(rowList.at(0).ID == 4);
    QVERIFY(rowList.at(0).StudentForeName == "Vorname1-10a");
    QVERIFY(rowList.at(0).GiveBackRow.ID == 2);
    QVERIFY(rowList.at(0).GiveBackRow.Damage == "Wasserschaden");
    QVERIFY(rowList.at(0).GiveBackRow.Date == "20.10.2017");
    QVERIFY(rowList.at(1).ID == 5);
    QVERIFY(rowList.at(1).StudentForeName == "Vorname1-5b");
    QVERIFY(rowList.at(1).GiveBackRow.ID == 3);
    QVERIFY(rowList.at(1).GiveBackRow.Damage == "Zerknittert");
    QVERIFY(rowList.at(1).GiveBackRow.Date == "20.10.2017");
    QVERIFY(rowList.at(2).ID == 6);
    QVERIFY(rowList.at(2).StudentForeName == "Vorname1-5b");
    QVERIFY(rowList.at(2).GiveBackRow.ID == 4);
    QVERIFY(rowList.at(2).GiveBackRow.Damage == "Wasserschaden");
    QVERIFY(rowList.at(2).GiveBackRow.Date == "20.10.2017");
}

//--------------------
// testGetRowsByDate: tests for an existing giveback and a not existing giveback
//--------------------
void TestLendingTable::testGetRowsByDate()
{
    // Existing
    QList<GiveBackTable::GiveBackRow> rowList = givebacktable->getRowsByDate("20.10.2017");
    QVERIFY(rowList.size() == 4);
    QVERIFY(rowList.at(0).ID == 1);
    QVERIFY(rowList.at(0).Damage.isEmpty());
    QVERIFY(rowList.at(0).Date == "20.10.2017");
    QVERIFY(rowList.at(1).ID == 2);
    QVERIFY(rowList.at(1).Damage == "Wasserschaden");
    QVERIFY(rowList.at(1).Date == "20.10.2017");
    QVERIFY(rowList.at(2).ID == 3);
    QVERIFY(rowList.at(2).Damage == "Zerknittert");
    QVERIFY(rowList.at(2).Date == "20.10.2017");
    QVERIFY(rowList.at(3).ID == 4);
    QVERIFY(rowList.at(3).Damage == "Wasserschaden");
    QVERIFY(rowList.at(3).Date == "20.10.2017");

    // Not Existing
    rowList = givebacktable->getRowsByDate("12.12.2017");
    QVERIFY(rowList.size() == 0);
}

//--------------------
// testSearchRowsByDamage: tests for an existing giveback and a not existing giveback
//--------------------
void TestLendingTable::testSearchRowsByDamage()
{
    // Existing
    QList<GiveBackTable::GiveBackRow> rowList = givebacktable->searchRowsByDamage("Wasserschaden");
    QVERIFY(rowList.size() == 2);
    QVERIFY(rowList.at(0).ID == 2);
    QVERIFY(rowList.at(0).Damage == "Wasserschaden");
    QVERIFY(rowList.at(0).Date == "20.10.2017");
    QVERIFY(rowList.at(1).ID == 4);
    QVERIFY(rowList.at(1).Damage == "Wasserschaden");
    QVERIFY(rowList.at(1).Date == "20.10.2017");

    // Not Existing
    rowList = givebacktable->searchRowsByDamage("Nicht");
    QVERIFY(rowList.size() == 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------
// testInsertLending: tests for a new lending, a duplicate book and a duplicate student
//--------------------
void TestLendingTable::testInsertLending()
{
    // New Entry
    int id = 0;
    QVERIFY(lendingtable->insertLending(&id, 0, "Vorname1-5a", "Nachname1-5a", "31.07.2007", "975-2", LendingTable::NORMAL, "23.05.2006", "24.06.2006"));
    testGetLending();
    LendingTable::LendingRow* row = lendingtable->getLending(7);
    QVERIFY(id == 7);
    QVERIFY(row->ID == 7);
    QVERIFY(row->StudentForeName == "Vorname1-5a");
    QVERIFY(row->StudentSurName == "Nachname1-5a");
    QVERIFY(row->StudentBirthday == "31.07.2007");
    QVERIFY(row->BookIsbn == "975-2");
    QVERIFY(row->type == LendingTable::NORMAL);
    QVERIFY(row->LendDate == "23.05.2006");
    QVERIFY(row->BringBackDate == "24.06.2006");
    QVERIFY(row->GiveBackRow.ID == 0);
    QVERIFY(row->GiveBackRow.Damage == "");
    QVERIFY(row->GiveBackRow.Date == "");

    // Duplicate book and student
    id = 0;
    QVERIFY(lendingtable->insertLending(&id, 0, "Vorname1-5a", "Nachname1-5a", "31.07.2007", "975-2", LendingTable::NORMAL, "23.05.2006", "24.06.2006"));
    row = lendingtable->getLending(8);
    QVERIFY(id == 8);
    QVERIFY(row->ID == 8);
    QVERIFY(row->StudentForeName == "Vorname1-5a");
    QVERIFY(row->StudentSurName == "Nachname1-5a");
    QVERIFY(row->StudentBirthday == "31.07.2007");
    QVERIFY(row->BookIsbn == "975-2");
    QVERIFY(row->type == LendingTable::NORMAL);
    QVERIFY(row->LendDate == "23.05.2006");
    QVERIFY(row->BringBackDate == "24.06.2006");
    QVERIFY(row->GiveBackRow.ID == 0);
    QVERIFY(row->GiveBackRow.Damage == "");
    QVERIFY(row->GiveBackRow.Date == "");

    // TODO Currently not working like expected, siehe issue in gitlab
    // Not Existing student
    //QVERIFY(lendingtable->insertLending(0, "Not-5a", "Not-5a", "31.07.2007", "975-2", LendingTable::NORMAL, "23.05.2006", "24.06.2006") == false);

    // Not Existing book
    //QVERIFY(lendingtable->insertLending(0, "Vorname1-5a", "Nachname1-5a", "31.07.2007", "999-2", LendingTable::NORMAL, "23.05.2006", "24.06.2006") == false);
}

//--------------------
// testInsertLending2: tests for a new lending
//--------------------
void TestLendingTable::testInsertLending2()
{
    // New Entry
    LendingTable::LendingRow newRow = {0, GiveBackTable::GiveBackRow{0, "", ""}, "Vorname2-5a", "Nachname2-5a", "29.08.2006",  "975-1", LendingTable::HOLIDAY, "23.05.2009", "24.06.2009"};
    QVERIFY(lendingtable->insertLending(&newRow));
    testGetLending();
    LendingTable::LendingRow* row = lendingtable->getLending(9);
    QVERIFY(row->ID == 9);
    QVERIFY(row->StudentForeName == "Vorname2-5a");
    QVERIFY(row->StudentSurName == "Nachname2-5a");
    QVERIFY(row->StudentBirthday == "29.08.2006");
    QVERIFY(row->BookIsbn == "975-1");
    QVERIFY(row->type == LendingTable::HOLIDAY);
    QVERIFY(row->LendDate == "23.05.2009");
    QVERIFY(row->BringBackDate == "24.06.2009");
    QVERIFY(row->GiveBackRow.ID == 0);
    QVERIFY(row->GiveBackRow.Damage == "");
    QVERIFY(row->GiveBackRow.Date == "");
}

//--------------------
// testInsertGiveBack: tests for a new giveback and a not existing lending
//--------------------
void TestLendingTable::testInsertGiveBack()
{
    // New entry
    GiveBackTable::GiveBackRow gbRow = {0, "Zerfetzt", "29.09.2007"};
    testGetLending();
    LendingTable::LendingRow* lRow = lendingtable->getLending(3);
    lRow->GiveBackRow = gbRow;
    QVERIFY(lendingtable->insertGiveBack(lRow));
    lRow = lendingtable->getLending(3);
    QVERIFY(lRow->ID == 3);
    QVERIFY(lRow->GiveBackRow.ID == 5);
    QVERIFY(lRow->GiveBackRow.Damage == "Zerfetzt");
    QVERIFY(lRow->GiveBackRow.Date == "29.09.2007");
    testGetGiveBack();
    GiveBackTable::GiveBackRow* newgbRow = givebacktable->getRow(5);
    QVERIFY(newgbRow->ID == 5);
    QVERIFY(newgbRow->Damage == "Zerfetzt");
    QVERIFY(newgbRow->Date == "29.09.2007");

    // Not existing lending
    lRow = new LendingTable::LendingRow{0, GiveBackTable::GiveBackRow{0, "", ""}, "Vorname2-5a", "Nachname2-5a", "29.08.2006", "975-1", LendingTable::HOLIDAY, "23.05.2009", "24.06.2009"};
    QVERIFY(lendingtable->insertGiveBack(lRow) == false);
}

//--------------------
// testRemoveLending: tests for a existing lending without giveback, with giveback and a not existing lending
//--------------------
void TestLendingTable::testRemoveLending()
{
    // Existing without giveback
    QVERIFY(lendingtable->removeLending(3));
    testGetLending();
    LendingTable::LendingRow* row = lendingtable->getLending(3);
    QVERIFY(row->ID == 0);
    QVERIFY(row->StudentForeName.isEmpty());
    QVERIFY(row->GiveBackRow.ID == 0);

    // Existing with giveback
    QVERIFY(lendingtable->removeLending(6));
    row = lendingtable->getLending(6);
    QVERIFY(row->ID == 0);
    QVERIFY(row->StudentForeName.isEmpty());
    QVERIFY(row->GiveBackRow.ID == 0);
    testGetGiveBack();
    GiveBackTable::GiveBackRow* gbRow = givebacktable->getRow(4);
    QVERIFY(gbRow->ID == 0);
    QVERIFY(gbRow->Damage.isEmpty());
    QVERIFY(gbRow->Date.isEmpty());

    // Not Existing
    QVERIFY(lendingtable->removeLending(99));
    row = lendingtable->getLending(99);
    QVERIFY(row->ID == 0);
    QVERIFY(row->StudentForeName.isEmpty());
    QVERIFY(row->GiveBackRow.ID == 0);
}

//--------------------
// testUpdateGiveBackDamage: tests for a existing giveback and a not existing giveback
//--------------------
void TestLendingTable::testUpdateGiveBackDamage()
{
    // Existing
    QVERIFY(lendingtable->updateGiveBackDamage(1, "Kaputter Einband"));
    testGetGiveBack();
    GiveBackTable::GiveBackRow* row = givebacktable->getRow(1);
    QVERIFY(row->ID == 1);
    QVERIFY(row->Damage == "Kaputter Einband");
    QVERIFY(row->Date == "20.10.2017");

    // Not Existing
    QVERIFY(lendingtable->updateGiveBackDamage(99, "Kaputter Einband"));
    row = givebacktable->getRow(99);
    QVERIFY(row->ID == 0);
    QVERIFY(row->Damage.isEmpty());
    QVERIFY(row->Date.isEmpty());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------
// cleanupTestCase: clean up and close database
//--------------------
void TestLendingTable::cleanupTestCase()
{
    // Save the name in a String, else a warning gets thrown
    QString defaultConnectionName = QSqlDatabase::database().connectionName();
    QSqlDatabase::removeDatabase(defaultConnectionName);
    delete lendingtable;
}
