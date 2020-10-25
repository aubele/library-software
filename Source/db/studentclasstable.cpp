#include "studentclasstable.h"

#include <QSQLQuery>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QVariant>


//========================================================
// Implementation of StudentClassTable
//========================================================

//--------------------
// Constructor
//--------------------
StudentClassTable::StudentClassTable(QString tablename, QString prenamecolumn, QString surnamecolumn, QString dateofbirthcolumn, QString classnamecolumn) noexcept:
    mTableName(tablename),
    mPreNameColumn(prenamecolumn),
    mSurNameColumn(surnamecolumn),
    mDateOfBirthColumn(dateofbirthcolumn),
    mClassNameColumn(classnamecolumn)
{
}

//------------------
// Destructor
//------------------
StudentClassTable::~StudentClassTable()
{
}

//--------------------
// insertStudentClass: adds a new studentclass
// Parameter: studentPreName, studentSurName, studentDateOfBirth, className
//--------------------
bool StudentClassTable::insertStudentClass(QString studentPreName, QString studentSurName, QString studentDateOfBirth, QString className) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO " + mTableName + " (" + mPreNameColumn + ", " + mSurNameColumn + ", " + mDateOfBirthColumn + ", " +
                   mClassNameColumn + ") VALUES ('" + studentPreName + "', '" + studentSurName + "', '" + studentDateOfBirth + "', '" + className +"');");

    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// removeStudentClass: removes a studentclass
// Parameter: studentPreName, studentSurName, studentDateOfBirth, className
//--------------------
bool StudentClassTable::removeStudentClass(QString studentPreName, QString studentSurName, QString studentDateOfBirth, QString className) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM " + mTableName + " WHERE " + mPreNameColumn + "='" + studentPreName + "' AND " + mSurNameColumn +
                  "='" + studentSurName + "' AND " + mDateOfBirthColumn + "='" + studentDateOfBirth + "' AND " + mClassNameColumn + "='" + className +"';");

    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// removeStudent: removes all classes from one student
// Parameter: studentPreName, studentSurName, studentDateOfBirth
//--------------------
bool StudentClassTable::removeStudent(QString studentPreName, QString studentSurName, QString studentDateOfBirth) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM " + mTableName + " WHERE " + mPreNameColumn + "='" + studentPreName + "' AND " + mSurNameColumn +
                  "='" + studentSurName + "' AND " + mDateOfBirthColumn + "='" + studentDateOfBirth + "';");

    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// removeClass: removes all entries with the given classname
// Parameter: className
//--------------------
bool StudentClassTable::removeClass(QString className) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
    {
        return false;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM " + mTableName + " WHERE " + mClassNameColumn + "='" + className +"';");

    if(query.exec())
    {
        success = true;
    }

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// getStudentClass: returns a studentclass
// Parameter: studentPreName, studentSurName, studentDateOfBirth, className
//--------------------
StudentClassTable::StudentClassRow* StudentClassTable::getStudentClass(QString studentPreName, QString studentSurName, QString studentDateOfBirth, QString className)
{
    if(!QSqlDatabase::database().open())
    {
        throw QString("Error in code: Method getStudentClass of StudentClassTable: Database could not be opened!");
    }
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mPreNameColumn + "='" + studentPreName + "' AND " + mSurNameColumn +
                    "='" + studentSurName + "' AND " + mDateOfBirthColumn + "='" + studentDateOfBirth + "' AND " + mClassNameColumn + "='" + className +"';");

    int idStudentPreName = query.record().indexOf(mPreNameColumn);
    int idStudentSurName = query.record().indexOf(mSurNameColumn);
    int idStudentDateOfBirth = query.record().indexOf(mDateOfBirthColumn);
    int idStudentClassName = query.record().indexOf(mClassNameColumn);

    StudentClassTable::StudentClassRow* row = new StudentClassTable::StudentClassRow();
    while( query.next())
    {
        row->studentPreName = query.value(idStudentPreName).toString();
        row->studentSurName = query.value(idStudentSurName).toString();
        row->studentDateOfBirth = query.value(idStudentDateOfBirth).toString();
        row->className = query.value(idStudentClassName).toString();
    }

    QSqlDatabase::database().close();
    return row;
}

//--------------------
// getClassNamesFromStudent: returns all classes from one student as stringlist
// Parameter: studentPreName, studentSurName, studentDateOfBirth
//--------------------
QStringList StudentClassTable::getClassNamesFromStudent(QString studentPreName, QString studentSurName, QString studentDateOfBirth, StudentClassTableSort sortoption)
{
    if(!QSqlDatabase::database().open())
    {
        throw QString("Error in code: Method getClassNamesFromStudent of StudentClassTable: Database could not be opened!");
    }
    QString sort = " ORDER BY ";
    switch(sortoption)
    {
    case StudentClassTable::ClassNameAscending:
        sort += mClassNameColumn + " ASC";
        break;
    case StudentClassTable::ClassNameDescending:
        sort += mClassNameColumn + " DESC";
        break;
    }
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mPreNameColumn + "='" + studentPreName + "' AND " + mSurNameColumn +
                    "='" + studentSurName + "' AND " + mDateOfBirthColumn + "='" + studentDateOfBirth + "'" + sort + ";");

    int idStudentClassName = query.record().indexOf(mClassNameColumn);

    QStringList classNames;
    while( query.next())
    {
        classNames.append(query.value(idStudentClassName).toString());
    }

    QSqlDatabase::database().close();
    return classNames;
}

//--------------------
// getStudents: returns all students which are in the specified class
// Parameter: className
//--------------------
QList<StudentClassTable::StudentClassRow> StudentClassTable::getStudents(QString className, StudentTableSort sortoption)
{
    if(!QSqlDatabase::database().open())
    {
        throw QString("Error in code: Method getStudents of StudentClassTable: Database could not be opened!");
    }

    QString sort = " ORDER BY ";
    switch(sortoption)
    {
    case StudentTableSort::StudentNameAscending:
        sort += mSurNameColumn + " ASC";
        break;
    case StudentTableSort::StudentNameDescending:
        sort += mSurNameColumn + " DESC";
        break;
    }

    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mClassNameColumn + "='" + className + "'" + sort + ";");

    int idStudentPreName = query.record().indexOf(mPreNameColumn);
    int idStudentSurName = query.record().indexOf(mSurNameColumn);
    int idStudentDateOfBirth = query.record().indexOf(mDateOfBirthColumn);
    int idStudentClassName = query.record().indexOf(mClassNameColumn);

    QList<StudentClassTable::StudentClassRow> rows;
    while( query.next())
    {
        StudentClassRow row;
        row.studentPreName = query.value(idStudentPreName).toString();
        row.studentSurName = query.value(idStudentSurName).toString();
        row.studentDateOfBirth = query.value(idStudentDateOfBirth).toString();
        row.className = query.value(idStudentClassName).toString();

        rows.append(row);
    }

    QSqlDatabase::database().close();
    return rows;
}
