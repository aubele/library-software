#ifndef LENDINGTABLE_H
#define LENDINGTABLE_H

#include <QString>
#include <QList>

#include <db_global.h>
#include "givebacktable.h"

class DBSHARED_EXPORT LendingTable
{
public:
    enum LendingType {NORMAL, HOLIDAY};

    struct LendingRow
    {
        int ID;
        GiveBackTable::GiveBackRow GiveBackRow;
        QString StudentForeName;
        QString StudentSurName;
        QString StudentBirthday;
        QString BookIsbn;
        LendingType type;
        QString LendDate;
        QString BringBackDate; // Wann das Buch zurückgegeben werden soll
    };

    LendingTable(GiveBackTable givebacktable, QString TableName, QString IdColumn, QString GiveBackIdColumn, QString LendDateColumn, QString BookIsbnColumn,
                 QString StudentSurNameColumn, QString StudentForNameColumn, QString StudentBirthdayColumn, QString BringBackDateColumn, QString TypeColumn);
    virtual ~LendingTable();

    virtual bool insertLending(LendingRow *newledingrow);
    virtual bool insertLending(int *id, int GiveBackId, QString forename, QString surname, QString studentbirthday, QString bookisbn, LendingType type, QString lenddate, QString bringbackdate);
    virtual bool insertGiveBack(LendingRow* row);
    virtual bool removeLending(int id);
    virtual bool updateGiveBackDamage(int id, QString damage);

    virtual LendingRow* getLending(int id);
    virtual LendingRow* getLendingFromGiveBackId(int id);
    virtual QList<LendingRow> getLendingsFromBook(QString bookisbn);
    virtual QList<LendingRow> getLendingsFromType(LendingType type);
    virtual QList<LendingRow> getLendingsFromStudent(QString studentforename, QString studentsurname, QString studentbirthday);
    virtual QList<LendingRow> getOpenLendingsFromStudent(QString studentforename, QString studentsurname, QString studentbirthday);
    virtual QList<LendingTable::LendingRow> getLendingsWithDamage();

private:
    GiveBackTable mGiveBackTable;
    QString mTableName;
    QString mIdColumn;
    QString mGiveBackIdColumn;
    QString mLendDateColumn;
    QString mBookIsbnColumn;
    QString mStudentSurNameColumn;
    QString mStudentForNameColumn;
    QString mStudentBirthdayColumn;
    QString mBringBackDateColumn;
    QString mTypeColumn;
};

#endif // LENDINGTABLE_H
