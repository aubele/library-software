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

    StudentClassTable(QString tablename, QString prenamecolumn, QString surnamecolumn, QString dateofbirthcolumn, QString classnamecolumn) noexcept;
    virtual ~StudentClassTable();

    virtual bool insertStudentClass(QString studentPreName, QString studentSurName, QString studentDateOfBirth, QString className) noexcept;

    virtual bool removeStudentClass(QString studentPreName, QString studentSurName, QString studentDateOfBirth, QString className) noexcept;
    virtual bool removeStudent(QString studentPreName, QString studentSurName, QString studentDateOfBirth) noexcept;
    virtual bool removeClass(QString className) noexcept;

    virtual StudentClassTable::StudentClassRow* getStudentClass(QString studentPreName, QString studentSurName, QString studentDateOfBirth, QString className);
    virtual QStringList getClassNamesFromStudent(QString studentPreName, QString studentSurName, QString studentDateOfBirth);
    virtual QList<StudentClassRow> getStudents(QString className);

private:
    QString mTableName;
    QString mPreNameColumn;
    QString mSurNameColumn;
    QString mDateOfBirthColumn;
    QString mClassNameColumn;
};

#endif // STUDENTCLASSTABLE_H
