#ifndef TESTUSERTABLE_H
#define TESTUSERTABLE_H

#include <QtTest>

#include "usertable.h"
#include "readsqlscript.h"


class TestUserTable : public QObject
{
    Q_OBJECT

public:
    TestUserTable();
    UserTable* usertable;

private Q_SLOTS:
    void initTestCase();

    void testGetUser();
    void testGetUsers();
    void testGetAllUsers();
    void testInsert();
    void testInsert2();
    void testRemove();

    void testUpdatePswd();
    void testUpdatePswdPerm();
    void testUpdatePerm();

    void cleanupTestCase();
};

#endif // TESTUSERTABLE_H
