#ifndef STUDENTCLASSTABLE_H
#define STUDENTCLASSTABLE_H

#include "QString"
#include "QList"
#include "db_global.h"

class DBSHARED_EXPORT StudentClassTable
{
public:
    struct StudentClassRow
    {
      QString studentPreName;
      QString studentSurName;
      QString studentDateOfBirth;
      QString className;
    };
    enum StudentClassTableSort {ClassNameAscending, ClassNameDescending};
    enum StudentTableSort {StudentNameAscending, StudentNameDescending};

    StudentClassTable(QString tablename, QString prenamecolumn, QString surnamecolumn, QString dateofbirthcolumn, QString classnamecolumn) noexcept;
    virtual ~StudentClassTable();

    virtual bool insertStudentClass(QString studentPreName, QString studentSurName, QString studentDateOfBirth, QString className) noexcept;

    virtual bool removeStudentClass(QString studentPreName, QString studentSurName, QString studentDateOfBirth, QString className) noexcept;
    virtual bool removeStudent(QString studentPreName, QString studentSurName, QString studentDateOfBirth) noexcept;
    virtual bool removeClass(QString className) noexcept;

    virtual StudentClassTable::StudentClassRow* getStudentClass(QString studentPreName, QString studentSurName, QString studentDateOfBirth, QString className);
    virtual QStringList getClassNamesFromStudent(QString studentPreName, QString studentSurName, QString studentDateOfBirth, StudentClassTableSort sortoption = StudentClassTable::ClassNameAscending);
    virtual QList<StudentClassRow> getStudents(QString className, StudentTableSort sortoption = StudentTableSort::StudentNameAscending);

private:
    QString mTableName;
    QString mPreNameColumn;
    QString mSurNameColumn;
    QString mDateOfBirthColumn;
    QString mClassNameColumn;
};

#endif // STUDENTCLASSTABLE_H
