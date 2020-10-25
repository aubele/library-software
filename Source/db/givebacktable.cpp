#include "givebacktable.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlRecord>

//=================
// Implementations of GiveBackTable
//================

//-----------------
// Constructor
//------------------
GiveBackTable::GiveBackTable(QString TableName, QString IdColumn, QString DamageColumn, QString DateColumn, QString PriceColumn):
    mIdColumn(IdColumn),
    mDamageColumn(DamageColumn),
    mDateColumn(DateColumn),
    mPriceColumn(PriceColumn),
    mTableName(TableName)
{
}

//------------------
// Destructor
//------------------
GiveBackTable::~GiveBackTable()
{
}

//------------------
// insert: inserts a new row into the database
// Parameter: newreturnrow
//------------------
bool GiveBackTable::insert(GiveBackRow* newreturnrow)
{
    return insert(&(newreturnrow->ID), newreturnrow->Date, newreturnrow->Damage);
}

//-----------------
// insert: inserts a new row into the database, writes the auto increment value in id
// Parameter: id, date, damage, price
//-----------------
bool GiveBackTable::insert(int* id, QString date, QString damage, QString price/*= ""*/)
{
    bool success = false;
    if(!QSqlDatabase::database().open())
        return false;

    QSqlQuery query;
    query.prepare("INSERT INTO " + mTableName + " (" + mIdColumn + ", " + mDateColumn + ", " + mDamageColumn + ", " + mPriceColumn +
                  ") VALUES (NULL, '" + date + "', '" + damage + "', '" + price + "');");

    if(query.exec())
           success = true;

    *id = query.lastInsertId().toInt();

    QSqlDatabase::database().close();
    return success;
}

//-----------------
// remove: removes the specified row from the database
// Parameter: id
//-----------------
bool GiveBackTable::remove(int id)
{
    bool success = false;
    if(!QSqlDatabase::database().open())
        return false;

    QSqlQuery query;
    query.prepare("DELETE FROM " + mTableName + " WHERE " + mIdColumn + "=" + QString::number(id) + ";");
    if(query.exec())
           success = true;

    QSqlDatabase::database().close();
    return success;
}

//-----------------
// updateDamage: writes a new value in damage for the specified id
// Parameter: id, damagenew
//-----------------
bool GiveBackTable::updateDamage(int id, QString damagenew)
{
    bool success = false;
    if(!QSqlDatabase::database().open())
        return false;

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mDamageColumn + "='" + damagenew + "' WHERE " +
            mIdColumn + " = " + QString::number(id) + ";");

    if(query.exec())
           success = true;

    QSqlDatabase::database().close();
    return success;
}

//-----------------
// updatePrice: writes a new value in price for the specified id
// Parameter: id, pricenew
//-----------------
bool GiveBackTable::updatePrice(int id, QString pricenew)
{
    bool success = false;
    if(!QSqlDatabase::database().open())
        return false;

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mPriceColumn + "='" + pricenew + "' WHERE " +
            mIdColumn + " = " + QString::number(id) + ";");

    if(query.exec())
           success = true;

    QSqlDatabase::database().close();
    return success;
}

//-----------------
// getRow: returns a GiveBackRow for the specified id
// Parameter: id
//-----------------
GiveBackTable::GiveBackRow* GiveBackTable::getRow(int id)
{
    if(!QSqlDatabase::database().open())
    {
        throw QString("Error in code: Method getRow of GiveBackTable: Database could not be opened!");
    }
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mIdColumn + "='" + QString::number(id) + "';");

    GiveBackRow* row = new GiveBackRow();
    while (query.next())
    {
       row->ID = query.value(query.record().indexOf(mIdColumn)).toInt();
       row->Damage = query.value(query.record().indexOf(mDamageColumn)).toString();
       row->Date = query.value(query.record().indexOf(mDateColumn)).toString();
    }
    QSqlDatabase::database().close();
    return row;
}

//-----------------
// getRowsByDate: returns a GiveBackRow for the specified date pattern
// Parameter: date
//-----------------
QList<GiveBackTable::GiveBackRow> GiveBackTable::getRowsByDate(QString date)
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method getRowsByDate of GiveBackTable: Database could not be opened!");
    }
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mDateColumn + "='" + date + "';");

    int idId = query.record().indexOf(mIdColumn);
    int idDate = query.record().indexOf(mDateColumn);
    int idDamage = query.record().indexOf(mDamageColumn);

    QList<GiveBackTable::GiveBackRow> rows;
    while (query.next())
    {
       GiveBackRow row;
       row.ID = query.value(idId).toInt();
       row.Date = query.value(idDate).toString();
       row.Damage = query.value(idDamage).toString();

       rows.append(row);
    }

    QSqlDatabase::database().close();
    return rows;
}

//-----------------
// getRowsWithDamage: returns all GiveBackRows which have a entry in damage
//-----------------
QList<GiveBackTable::GiveBackRow> GiveBackTable::getRowsWithDamage()
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method getRowsWithDamage of GiveBackTable: Database could not be opened!");
    }
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE NOT " + mDamageColumn + "='';");

    int idId = query.record().indexOf(mIdColumn);
    int idDate = query.record().indexOf(mDateColumn);
    int idDamage = query.record().indexOf(mDamageColumn);

    QList<GiveBackTable::GiveBackRow> rows;
    while (query.next())
    {
       GiveBackRow row;
       row.ID = query.value(idId).toInt();
       row.Date = query.value(idDate).toString();
       row.Damage = query.value(idDamage).toString();

       rows.append(row);
    }

    QSqlDatabase::database().close();
    return rows;
}

//-----------------
// searchRowsByDamage: returns a GiveBackRow, for the specified damage pattern
// Parameter: damage
//-----------------
QList<GiveBackTable::GiveBackRow> GiveBackTable::searchRowsByDamage(QString damage)
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method searchRowsByDamage of GiveBackTable: Database could not be opened!");
    }

    QSqlQuery pragmaQuery("PRAGMA case_sensitive_like = true;");
    pragmaQuery.exec();
    pragmaQuery.finish();

    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mDamageColumn + " LIKE '%" + damage + "%';");

    int idId = query.record().indexOf(mIdColumn);
    int idDate = query.record().indexOf(mDateColumn);
    int idDamage = query.record().indexOf(mDamageColumn);

    QList<GiveBackTable::GiveBackRow> rows;
    while (query.next())
    {
       GiveBackRow row;
       row.ID = query.value(idId).toInt();
       row.Date = query.value(idDate).toString();
       row.Damage = query.value(idDamage).toString();

       rows.append(row);
    }

    QSqlDatabase::database().close();
    return rows;
}
