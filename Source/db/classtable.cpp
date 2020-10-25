#include "classtable.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QVariant>

//====================
// Implementation of ClassTable
//====================

//--------------------
// Constructor
//--------------------
ClassTable::ClassTable(QString tablename, QString classnamecolumn, QString gradecolumn, QString subjectcolumn) noexcept :
    mTableName(tablename),
    mClassNameColumn(classnamecolumn),
    mGradeColumn(gradecolumn),
    mSubjectColumn(subjectcolumn)
{
}

//------------------
// Destructor
//------------------
ClassTable::~ClassTable() noexcept
{
}

//--------------------
// insert: adds a new class
// Parameter: className, grade, subject
//--------------------
bool ClassTable::insert(QString className, int grade, QString subject) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO " + mTableName + " (" + mClassNameColumn + ", " + mGradeColumn + ", " + mSubjectColumn
                  + ") VALUES ('" + className + "', '" + QString::number(grade) + "', '" + subject + "');");
    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// insert: adds a new class
// Parameter: classrow containing a class
//--------------------
bool ClassTable::insert(ClassRow classrow) noexcept
{
    return insert(classrow.className, classrow.grade, classrow.subject);
}

//--------------------
// update: updates the subject of a class
// Parameter: className and subject to update
//--------------------
bool ClassTable::update(QString className, QString subject) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mSubjectColumn + "='" + subject + "' WHERE " + mClassNameColumn +
                  "='" + className + "';");

    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// remove: adds a new class
// Parameter: className
//--------------------
bool ClassTable::remove(QString className) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM " + mTableName + " WHERE " + mClassNameColumn + "='" + className + "';");
    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// getClass: finds a class in the database and returns it as a ClassRow
// Parameter: className
//--------------------
ClassTable::ClassRow* ClassTable::getClass(QString className)
{
    if(!QSqlDatabase::database().open())
    {
        throw QString("Error in code: Method getClass of ClassTable: Database could not be opened!");
    }
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mClassNameColumn + "='" + className + "';");

    int idClassName = query.record().indexOf(mClassNameColumn);
    int idGrade = query.record().indexOf(mGradeColumn);
    int idSubject = query.record().indexOf(mSubjectColumn);

    ClassTable::ClassRow* row = new ClassTable::ClassRow();
    while (query.next())
    {
       row->className = query.value(idClassName).toString();
       row->grade = query.value(idGrade).toInt();
       row->subject = query.value(idSubject).toString();
    }

    QSqlDatabase::database().close();
    return row;
}

//--------------------
// getAllClasses: returns all rows in the ClassTable
//--------------------
QList<ClassTable::ClassRow> ClassTable::getAllClasses(ClassTableSort sortoption)
{
    if(!QSqlDatabase::database().open())
    {
        throw QString("Error in code: Method getAllClasses of ClassTable: Database could not be opened!");
    }
    QString sort = " ORDER BY ";
    switch(sortoption)
    {
    case ClassTable::ClassNameAscending:
        sort += mClassNameColumn + " ASC";
        break;
    case ClassTable::ClassNameDescending:
        sort += mClassNameColumn + " DESC";
        break;
    }
    QSqlQuery query("SELECT * FROM " + mTableName + sort + ";");

    int idClassName = query.record().indexOf(mClassNameColumn);
    int idGrade = query.record().indexOf(mGradeColumn);
    int idSubject = query.record().indexOf(mSubjectColumn);

    QList<ClassRow> rows;
    while (query.next())
    {
       ClassTable::ClassRow row;
       row.className = query.value(idClassName).toString();
       row.grade = query.value(idGrade).toInt();
       row.subject = query.value(idSubject).toString();

       rows.append(row);
    }

    QSqlDatabase::database().close();
    return rows;
}
