#ifndef CLASSTABLE_H
#define CLASSTABLE_H
#include <QString>
#include <QList>
#include "db_global.h"

class DBSHARED_EXPORT ClassTable
{
public:
    struct ClassRow
    {
        QString className;
        int grade;
        QString subject;
    };
    enum ClassTableSort {ClassNameAscending, ClassNameDescending};

    ClassTable(QString tablename, QString classnamecolumn, QString gradecolumn, QString subjectcolumn) noexcept;
    virtual ~ClassTable() noexcept;

    virtual bool insert(ClassRow classrow) noexcept;
    virtual bool insert(QString className, int grade, QString subject) noexcept;
    virtual bool update(QString className, QString subject) noexcept;
    virtual bool remove(QString className) noexcept;

    virtual ClassTable::ClassRow* getClass(QString className); // Only needed for the studenttable + unittests
    virtual QList<ClassTable::ClassRow> getAllClasses(ClassTableSort sortoption = ClassTable::ClassNameAscending);

private:
    QString mTableName;
    QString mClassNameColumn;
    QString mGradeColumn;
    QString mSubjectColumn;
};

#endif // CLASSTABLE_H
