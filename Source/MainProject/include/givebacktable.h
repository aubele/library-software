#ifndef RETURNTABLE_H
#define RETURNTABLE_H

#include <QString>
#include "db_global.h"

class DBSHARED_EXPORT GiveBackTable
{
public:
    struct GiveBackRow
    {
        int ID;
        QString Damage;
        QString Date;
        QString Price;
    };

    GiveBackTable(QString TableName, QString IdColumn, QString DamageColumn, QString DateColumn, QString PriceColumn);
    virtual ~GiveBackTable();

    virtual bool insert(GiveBackRow* newreturnrow);
    virtual bool insert(int* id, QString date, QString damage, QString price = "");
    virtual bool remove(int id); // Needed for the lendingtable in db api
    virtual bool updateDamage(int id, QString damagenew);
    virtual bool updatePrice(int id, QString price);

    virtual GiveBackRow* getRow(int id); // For lendingtable api
    virtual QList<GiveBackRow> getRowsByDate(QString date);
    virtual QList<GiveBackRow> getRowsWithDamage();
    virtual QList<GiveBackRow> searchRowsByDamage(QString damage);

private:
    QString mIdColumn;
    QString mDamageColumn;
    QString mDateColumn;
    QString mPriceColumn;
    QString mTableName;
};

#endif // RETURNTABLE_H
