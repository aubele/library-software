#ifndef TESTSTUDENTTABLE_H
#define TESTSTUDENTTABLE_H

#include <QtTest>

#include "studenttable.h"
#include "readsqlscript.h"


class TestStudentTable : public QObject
{
     Q_OBJECT

public:
    TestStudentTable();
    StudentTable* studenttable;
    StudentClassTable* studentclasstable;
    ClassTable* classtable;

private Q_SLOTS:
    void initTestCase();

    void testGetStudent();
    void testGetAllStudents();
    void testGetByClassName();
    void testGetClassesFromStudent();
    void testGetAllClasses();

    void testInsertStudent();
    void testInsertStudent2();
    void testInsertClass();

    void testRemoveStudent();
    void testRemoveClassFromStudent();
    void testRemoveClass();

    void testSearchPreName();
    void testSearchSurName();
    void testSearchComment();

    void testUpdateAddress();
    void testUpdateComment();
    void testUpdateClasses();

    void cleanupTestCase();
};

#endif // TESTSTUDENTTABLE_H
