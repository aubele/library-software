#ifndef STUDENTTABLE_H
#define STUDENTTABLE_H

#include <QString>
#include <QList>
#include "db_global.h"

#include "studentclasstable.h"
#include "classtable.h"

class DBSHARED_EXPORT StudentTable
{
public:
    struct StudentRow
    {
        QString preName;
        QString surName;
        QString address;
        QString dateOfBirth;
        QString comment;
        QList<ClassTable::ClassRow> classRows;

        bool operator==(StudentRow row) const
        {
            if(row.preName == preName && row.surName == surName && row.dateOfBirth == dateOfBirth)
                return true;
            else
                return false;
        }
    };
    enum StudentTableSort {StudentNameAscending, StudentNameDescending};
    StudentTable(StudentClassTable studentclasstable, ClassTable classtable, QString tablename, QString prenamecolumn, QString surnamecolumn,
                 QString addresscolumn, QString dateofbirthcolumn, QString commentcolumn) noexcept;
    virtual ~StudentTable() noexcept;

    virtual bool insertStudent(StudentRow student) noexcept;
    virtual bool insertStudent(QString preName, QString surName, QString address, QString dateOfBirth, QString comment, ClassTable::ClassRow classRow) noexcept;
    virtual bool insertStudentClass(QString preName, QString surName, QString dateOfBirth, ClassTable::ClassRow classRow) noexcept;

    virtual bool removeStudent(QString preName, QString surName, QString dateOfBirth) noexcept;
    virtual bool removeStudentClass(QString preName, QString surName, QString dateOfBirth, QString className) noexcept;
    virtual bool removeClass(QString className) noexcept;

    virtual StudentTable::StudentRow* getStudent(QString preName, QString surName, QString dateOfBirth);
    virtual QList<StudentRow> getAllStudents(StudentTableSort studentsorting = StudentTable::StudentNameAscending, StudentClassTable::StudentClassTableSort classsorting = StudentClassTable::ClassNameAscending);
    virtual QList<StudentRow> getStudentsByClassName(QString className, StudentClassTable::StudentTableSort sortoption = StudentClassTable::StudentNameAscending);
    virtual QList<ClassTable::ClassRow> getClassesFromStudent(QString preName, QString surName, QString dateOfBirth, StudentClassTable::StudentClassTableSort sortoption = StudentClassTable::ClassNameAscending);

    virtual QList<StudentRow> searchStudentsByPreName(QString preName);
    virtual QList<StudentRow> searchStudentsBySurName(QString surName);
    virtual QList<StudentRow> searchStudentsByComment(QString comment);

    virtual bool updateStudentAddress(QString preName, QString surName, QString dateOfBirth, QString address) noexcept;
    virtual bool updateStudentComment(QString preName, QString surName, QString dateOfBirth, QString comment) noexcept;
    virtual bool updateStudentClasses(QString preName, QString surName, QString dateOfBirth, QList<ClassTable::ClassRow> oldClassNameList, QList<ClassTable::ClassRow> newClassList) noexcept;

private:
    StudentClassTable mStudentClassTable;
    ClassTable mClassTable;
    QString mTableName;
    QString mPreNameColumn;
    QString mSurNameColumn;
    QString mAddressColumn;
    QString mDateOfBirthColumn;
    QString mCommentColumn;
};

#endif // STUDENTTABLE_H
