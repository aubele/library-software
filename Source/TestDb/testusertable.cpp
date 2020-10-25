#include "testusertable.h"


//--------------------
// Constructor
//--------------------
TestUserTable::TestUserTable()
{
    usertable = new UserTable("UserTable", "UserName", "Passwd", "Permissions");
}

//--------------------
// initTestCase: initiates the testdatabase via sql-scriptfile
//--------------------
void TestUserTable::initTestCase()
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
// testGetUser: tests for an existing user and a not existing user
//--------------------
void TestUserTable::testGetUser()
{
    // Existing
    UserTable::UserRow* row = usertable->getUser("Welcome");
    QVERIFY(row->UserName == "Welcome");
    QVERIFY(row->Passwd == "83218ac34c1834c26781fe4bde918ee4");
    QVERIFY(row->Permissions == Lending);

    // Not Existing
    row = usertable->getUser("GibtsNicht");
    QVERIFY(row->UserName.isEmpty());
}

//--------------------
// testGetUsers: tests for a present permission and a not present permission
//--------------------
void TestUserTable::testGetUsers()
{
    // Existing
    QList<UserTable::UserRow> list = usertable->getUsers(Lending);
    QVERIFY(list.size() == 2);
    QVERIFY(list.at(0).UserName == "Welcome");
    QVERIFY(list.at(1).UserName == "Hello");

    // Empty
    list = usertable->getUsers(Nothing);
    QVERIFY(list.size() == 0);
}

//--------------------
// testGetAllUsers: tests for all present users
//--------------------
void TestUserTable::testGetAllUsers()
{
    QList<UserTable::UserRow> list = usertable->getAllUsers();
    QVERIFY(list.size() == 3);
    QVERIFY(list.at(0).UserName == "Welcome");
    QVERIFY(list.at(1).UserName == "Test");
    QVERIFY(list.at(2).UserName == "Hello");
}

//--------------------
// testInsert: tests for a new entry and for an already existing entry
//--------------------
void TestUserTable::testInsert()
{
    // New Entry
    QVERIFY(usertable->insert("Fabio", "test", Adding));
    testGetUser();
    UserTable::UserRow* row = usertable->getUser("Fabio");
    QVERIFY(row->UserName == "Fabio");
    QVERIFY(row->Passwd == "test");
    QVERIFY(row->Permissions == Adding);

    // Already existing entry
    QVERIFY(usertable->insert("Fabio", "testNeu", Administrative) == false);
}

//--------------------
// testInsert2: tests for a new entry and an already existing entry
//--------------------
void TestUserTable::testInsert2()
{
    // New Entry
    QVERIFY(usertable->insert(UserTable::UserRow{"Aubele", "test2", Administrative}));
    testGetUser();
    UserTable::UserRow* row = usertable->getUser("Aubele");
    QVERIFY(row->UserName == "Aubele");
    QVERIFY(row->Passwd == "test2");
    QVERIFY(row->Permissions == Administrative);

    // Already existing entry
    QVERIFY(usertable->insert(UserTable::UserRow{"Aubele", "testNeu", Lending}) == false);
}

//--------------------
// testRemove: tests for an existing entry and a not existing entry
//--------------------
void TestUserTable::testRemove()
{
    // Existing
    QVERIFY(usertable->remove("Hello"));
    testGetUser();
    UserTable::UserRow* row = usertable->getUser("Hello");
    QVERIFY(row->UserName.isEmpty());

    // Not Existing -> Returns true even if its not existing, but doesnt remove anyt
    QVERIFY(usertable->remove("GibtsNicht"));
    row = usertable->getUser("GibtsNicht");
    QVERIFY(row->UserName.isEmpty());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------
// testUpdatePswd: tests for an existing entry and a not existing entry
//--------------------
void TestUserTable::testUpdatePswd()
{
    // Existing
    QVERIFY(usertable->update("Test", "neu"));
    testGetUser();
    UserTable::UserRow* row = usertable->getUser("Test");
    QVERIFY(row->Passwd == "neu");

    // Not existing -> returns true, but doesnt do anything
    QVERIFY(usertable->update("GibtsNicht", "update"));
    row = usertable->getUser("GibtsNicht");
    QVERIFY(row->UserName.isEmpty());
    QVERIFY(row->Passwd.isEmpty());
}

//--------------------
// testUpdatePswdPerm: tests for an existing entry and a not existing entry
//--------------------
void TestUserTable::testUpdatePswdPerm()
{
    // Existing
    usertable->update("Test", "neuer", Administrative);
    testGetUser();
    UserTable::UserRow* row = usertable->getUser("Test");
    QVERIFY(row->Passwd == "neuer");
    QVERIFY(row->Permissions == Administrative);

    // Not existing -> returns true, but doesnt do anything
    QVERIFY(usertable->update("GibtsNicht", "update", Nothing));
    row = usertable->getUser("GibtsNicht");
    QVERIFY(row->UserName.isEmpty());
    QVERIFY(row->Passwd.isEmpty());
    QVERIFY(row->Permissions == 0);
}

//--------------------
// testUpdatePerm: tests for an existing entry and a not existing entry
//--------------------
void TestUserTable::testUpdatePerm()
{
    // Existing
    usertable->update("Test", Nothing);
    testGetUser();
    UserTable::UserRow* row = usertable->getUser("Test");
    QVERIFY(row->Permissions == Nothing);

    // Not existing -> returns true, but doesnt do anything
    QVERIFY(usertable->update("GibtsNicht", Lending));
    row = usertable->getUser("GibtsNicht");
    QVERIFY(row->UserName.isEmpty());
    QVERIFY(row->Permissions == 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------
// cleanupTestCase: clean up and close database
//--------------------
void TestUserTable::cleanupTestCase()
{
    // Save the name in a String, else a warning gets thrown
    QString defaultConnectionName = QSqlDatabase::database().connectionName();
    QSqlDatabase::removeDatabase(defaultConnectionName);
    delete usertable;
}
