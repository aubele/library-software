#include "lendingtable.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlRecord>

//=======================
// Implementations of LendingTable
//=======================

//-----------------------
// Constructor
//-----------------------
LendingTable::LendingTable(GiveBackTable givebacktable, QString TableName, QString IdColumn, QString GiveBackIdColumn, QString LendDateColumn, QString BookIsbnColumn,
                           QString StudentSurNameColumn, QString StudentForNameColumn, QString StudentBirthdayColumn, QString BringBackDateColumn, QString TypeColumn) :
mGiveBackTable(givebacktable),
mTableName(TableName),
mIdColumn(IdColumn),
mGiveBackIdColumn(GiveBackIdColumn),
mLendDateColumn(LendDateColumn),
mBookIsbnColumn(BookIsbnColumn),
mStudentSurNameColumn(StudentSurNameColumn),
mStudentForNameColumn(StudentForNameColumn),
mStudentBirthdayColumn(StudentBirthdayColumn),
mBringBackDateColumn(BringBackDateColumn),
mTypeColumn(TypeColumn)
{
}

//----------------------
// Destructor
//----------------------
LendingTable::~LendingTable()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//-----------------------
// insertLending: adds a new lendingrow to the database
// Parameter: newlendingrow
//-----------------------
bool LendingTable::insertLending(LendingRow *newlendingrow)
{
    return insertLending(&newlendingrow->ID, newlendingrow->GiveBackRow.ID, newlendingrow->StudentForeName, newlendingrow->StudentSurName, newlendingrow->StudentBirthday,
                         newlendingrow->BookIsbn, newlendingrow->type, newlendingrow->LendDate, newlendingrow->BringBackDate);
}

//-----------------------
// insertLending: adds a new entry in the database
// Parameter: giveBackId, forename, surname, studentbirthday, bookisbn, type, lenddate, bringbackdate
//----------------------
bool LendingTable::insertLending(int *id, int giveBackId, QString forename, QString surname, QString studentbirthday, QString bookisbn, LendingType type, QString lenddate, QString bringbackdate)
{
    // TODO: should return false if the student or the book doesnt exist

    bool success = false;
    if(!QSqlDatabase::database().open())
        return false;

    QSqlQuery query;
    query.prepare("INSERT INTO " + mTableName + " (" + mIdColumn + ", " + mGiveBackIdColumn + ", " + mLendDateColumn + ", " + mBookIsbnColumn + ", " + mStudentSurNameColumn
                  + ", " + mStudentForNameColumn + ", " + mStudentBirthdayColumn + ", " + mTypeColumn + + ", " + mBringBackDateColumn + ") VALUES (NULL, '"
                  + QString::number(giveBackId) + "', '" + lenddate + "', '" + bookisbn + "', '" + surname + "', '" + forename + "', '" + studentbirthday + "', '" +
                  QString::number(static_cast<int>(type)) + "', '" + bringbackdate + "');");

    if(query.exec())
           success = true;

    *id = query.lastInsertId().toInt();

    QSqlDatabase::database().close();
    return success;
}

//---------------------
// insertGiveBack: inserts an entry in the givebacktable and updates the givebackid in the lendingtable
// Parameter: lendingrow
//---------------------
bool LendingTable::insertGiveBack(LendingRow* lendingrow)
{
    bool success = false;

    // Check if lendingrow exist
    if(getLending(lendingrow->ID)->ID == 0)
        return false;

    // Insert the givebackrow
    if(!mGiveBackTable.insert(&lendingrow->GiveBackRow))
        return false;

    if(!QSqlDatabase::database().open())
        return false;

    // Update the new givebackid in the lendingtable
    int newGiveBackId = lendingrow->GiveBackRow.ID;

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mGiveBackIdColumn + " = '" + QString::number(newGiveBackId) +
                  "' WHERE " + mIdColumn + " = '" + QString::number(lendingrow->ID) + "';");

    if(query.exec())
           success = true;

    QSqlDatabase::database().close();
    return success;
}

//----------------------
// removeLending: removes the specified lendingrow from the database
// Parameter: id
//----------------------
bool LendingTable::removeLending(int id)
{
    bool success = false;

    // First remove the giveback
    LendingRow* row = getLending(id);
    if(row->GiveBackRow.ID != 0)
    {
        if(!mGiveBackTable.remove(row->GiveBackRow.ID))
            return false;
    }

    if(!QSqlDatabase::database().open())
        return false;

    // Then remove the lending
    QSqlQuery query;
    query.prepare("DELETE FROM " + mTableName + " WHERE " + mIdColumn + "='" + QString::number(id) + "';");
    if(query.exec())
           success = true;

    QSqlDatabase::database().close();
    return success;
}

//----------------------
// updateGiveBackDamage:  writes a new value in damage in the givebacktable for the specified id
// Parameter: id, damage
//----------------------
bool LendingTable::updateGiveBackDamage(int id, QString damage)
{
    return mGiveBackTable.updateDamage(id, damage);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//----------------------
// getLending: returns a specified lendingrow
// Parameter: id
//---------------------
LendingTable::LendingRow* LendingTable::getLending(int id)
{
    if(!QSqlDatabase::database().open())
        throw QString("Error in code: Method getLending of LendingTable: Database could not be opened!");

    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mIdColumn + "='" + QString::number(id) + "';");

    LendingRow* row = new LendingRow();
    while (query.next())
    {
       row->ID = query.value(query.record().indexOf(mIdColumn)).toInt();
       row->BookIsbn = query.value(query.record().indexOf(mBookIsbnColumn)).toString();
       row->BringBackDate = query.value(query.record().indexOf(mBringBackDateColumn)).toString();
       row->LendDate = query.value(query.record().indexOf(mLendDateColumn)).toString();
       row->StudentBirthday = query.value(query.record().indexOf(mStudentBirthdayColumn)).toString();
       row->StudentForeName = query.value(query.record().indexOf(mStudentForNameColumn)).toString();
       row->StudentSurName = query.value(query.record().indexOf(mStudentSurNameColumn)).toString();
       row->type = static_cast<LendingType>(query.value(query.record().indexOf(mTypeColumn)).toInt());
       row->GiveBackRow.ID = query.value(query.record().indexOf(mGiveBackIdColumn)).toInt();
    }

    // Get GiveBackRows  outside of the loop
    if(row->GiveBackRow.ID != 0)
    {
        GiveBackTable::GiveBackRow* givebackrow = mGiveBackTable.getRow(row->GiveBackRow.ID);
        row->GiveBackRow = *givebackrow;
    }

    QSqlDatabase::database().close();
    return row;
}

//----------------------
// getLending: returns a lendingrow specified by the given givebackid
// Parameter: id
//---------------------
LendingTable::LendingRow* LendingTable::getLendingFromGiveBackId(int id)
{
    LendingRow* row = new LendingRow();

    // No entry if the id is zero since this signalizes an empty entry
    if(id == 0)
        return row;

    if(!QSqlDatabase::database().open())
        throw QString("Error in code: Method getLendingByGiveBackId of LendingTable: Database could not be opened!");

    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mGiveBackIdColumn + "='" + QString::number(id) + "';");

    while (query.next())
    {
       row->ID = query.value(query.record().indexOf(mIdColumn)).toInt();
       row->BookIsbn = query.value(query.record().indexOf(mBookIsbnColumn)).toString();
       row->BringBackDate = query.value(query.record().indexOf(mBringBackDateColumn)).toString();
       row->LendDate = query.value(query.record().indexOf(mLendDateColumn)).toString();
       row->StudentBirthday = query.value(query.record().indexOf(mStudentBirthdayColumn)).toString();
       row->StudentForeName = query.value(query.record().indexOf(mStudentForNameColumn)).toString();
       row->StudentSurName = query.value(query.record().indexOf(mStudentSurNameColumn)).toString();
       row->type = static_cast<LendingType>(query.value(query.record().indexOf(mTypeColumn)).toInt());
       row->GiveBackRow.ID = query.value(query.record().indexOf(mGiveBackIdColumn)).toInt();
    }

    // Get GiveBackRows outside of the loop
    if(row->GiveBackRow.ID != 0)
    {
        GiveBackTable::GiveBackRow* givebackrow = mGiveBackTable.getRow(row->GiveBackRow.ID);
        row->GiveBackRow = *givebackrow;
    }

    QSqlDatabase::database().close();
    return row;
}

//----------------------
// getLendingsFromBook: returns all lendingrows specified by the given book
// Parameter: bookisbn
//---------------------
QList<LendingTable::LendingRow> LendingTable::getLendingsFromBook(QString bookisbn)
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method getLendingsFromBook of LendingTable: Database could not be opened!");
    }

    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mBookIsbnColumn + "='" + bookisbn + + "'" +
                    "AND " + mGiveBackIdColumn + "=0" + ";");

    QList<LendingTable::LendingRow> rows;
    while (query.next())
    {
       LendingRow row;
       row.BookIsbn = query.value(query.record().indexOf(mBookIsbnColumn)).toString();
       row.BringBackDate = query.value(query.record().indexOf(mBringBackDateColumn)).toString();
       row.LendDate = query.value(query.record().indexOf(mLendDateColumn)).toString();
       row.ID = query.value(query.record().indexOf(mIdColumn)).toInt();
       row.GiveBackRow.ID = query.value(query.record().indexOf(mGiveBackIdColumn)).toInt();
       row.StudentBirthday = query.value(query.record().indexOf(mStudentBirthdayColumn)).toString();
       row.StudentForeName = query.value(query.record().indexOf(mStudentForNameColumn)).toString();
       row.StudentSurName = query.value(query.record().indexOf(mStudentSurNameColumn)).toString();
       row.type = static_cast<LendingType>(query.value(query.record().indexOf(mTypeColumn)).toInt());

       rows.append(row);
    }

    // Get GiveBackRows outside of the loop
    QList<LendingTable::LendingRow> returnrows;
    for(LendingRow row : rows)
    {
        if(row.GiveBackRow.ID != 0)
        {
            GiveBackTable::GiveBackRow* givebackrow = mGiveBackTable.getRow(row.GiveBackRow.ID);
            row.GiveBackRow = *givebackrow;
            returnrows.append(row);
        }
        else
        {
            returnrows.append(row);
        }
    }

    QSqlDatabase::database().close();
    return returnrows;
}

//----------------------
// getLendingsFromType: returns all lendingrows specified by the given type
// Parameter: type
//---------------------
QList<LendingTable::LendingRow> LendingTable::getLendingsFromType(LendingType type)
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method getLendingsFromType of LendingTable: Database could not be opened!");
    }

    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mTypeColumn + "='" + QString::number(static_cast<int>(type)) + "';");

    QList<LendingTable::LendingRow> rows;
    while (query.next())
    {
       LendingRow row;
       row.BookIsbn = query.value(query.record().indexOf(mBookIsbnColumn)).toString();
       row.BringBackDate = query.value(query.record().indexOf(mBringBackDateColumn)).toString();
       row.LendDate = query.value(query.record().indexOf(mLendDateColumn)).toString();
       row.ID = query.value(query.record().indexOf(mIdColumn)).toInt();
       row.GiveBackRow.ID = query.value(query.record().indexOf(mGiveBackIdColumn)).toInt();
       row.StudentBirthday = query.value(query.record().indexOf(mStudentBirthdayColumn)).toString();
       row.StudentForeName = query.value(query.record().indexOf(mStudentForNameColumn)).toString();
       row.StudentSurName = query.value(query.record().indexOf(mStudentSurNameColumn)).toString();
       row.type = static_cast<LendingType>(query.value(query.record().indexOf(mTypeColumn)).toInt());

       rows.append(row);
    }

    // Get GiveBackRows outside of the loop
    QList<LendingTable::LendingRow> returnrows;
    for(LendingRow row : rows)
    {
        if(row.GiveBackRow.ID != 0)
        {
            GiveBackTable::GiveBackRow* givebackrow = mGiveBackTable.getRow(row.GiveBackRow.ID);
            row.GiveBackRow = *givebackrow;
            returnrows.append(row);
        }
        else
        {
            returnrows.append(row);
        }
    }

    QSqlDatabase::database().close();
    return returnrows;
}

//----------------------
// getLendingsFromStudent: returns a list of ReturnRows from a specified students
// Parameter: studentsurname, studentforename, studentbirthday
//---------------------
QList<LendingTable::LendingRow> LendingTable::getLendingsFromStudent(QString studentforename, QString studentsurname, QString studentbirthday)
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method getLendingsFromStudent of LendingTable: Database could not be opened!");
    }

    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mStudentForNameColumn + "='" + studentforename + "' AND " + mStudentSurNameColumn
                    + "='" + studentsurname + "' AND " + mStudentBirthdayColumn + "='" + studentbirthday + "';");

    QList<LendingTable::LendingRow> rows;
    while (query.next())
    {
       LendingRow row;
       row.BookIsbn = query.value(query.record().indexOf(mBookIsbnColumn)).toString();
       row.BringBackDate = query.value(query.record().indexOf(mBringBackDateColumn)).toString();
       row.LendDate = query.value(query.record().indexOf(mLendDateColumn)).toString();
       row.ID = query.value(query.record().indexOf(mIdColumn)).toInt();
       row.GiveBackRow.ID = query.value(query.record().indexOf(mGiveBackIdColumn)).toInt();
       row.StudentBirthday = query.value(query.record().indexOf(mStudentBirthdayColumn)).toString();
       row.StudentForeName = query.value(query.record().indexOf(mStudentForNameColumn)).toString();
       row.StudentSurName = query.value(query.record().indexOf(mStudentSurNameColumn)).toString();
       row.type = static_cast<LendingType>(query.value(query.record().indexOf(mTypeColumn)).toInt());

       rows.append(row);
    }

    // Get GiveBackRows outside of the loop
    QList<LendingTable::LendingRow> returnrows;
    for(LendingRow row : rows)
    {
        if(row.GiveBackRow.ID != 0)
        {
            GiveBackTable::GiveBackRow* givebackrow = mGiveBackTable.getRow(row.GiveBackRow.ID);
            row.GiveBackRow = *givebackrow;
            returnrows.append(row);
        }
        else
        {
            returnrows.append(row);
        }
    }

    QSqlDatabase::database().close();
    return returnrows;
}

//----------------------
// getLendingsFromStudent: returns a list of lendingrows from a specified students without a specified givebackid
// Parameter: studentsurname, studentforename, studentbirthday
//---------------------
QList<LendingTable::LendingRow> LendingTable::getOpenLendingsFromStudent(QString studentforename, QString studentsurname, QString studentbirthday)
{
    if(!QSqlDatabase::database().open())
    {
       throw QString("Error in code: Method getLendingsFromStudent of LendingTable: Database could not be opened!");
    }

    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mStudentForNameColumn + "='" + studentforename + "' AND " + mStudentSurNameColumn
                    + "='" + studentsurname + "' AND " + mStudentBirthdayColumn + "='" + studentbirthday + "' AND " + mGiveBackIdColumn + "=" + QString::number(0) + ";");

    QList<LendingTable::LendingRow> rows;
    while (query.next())
    {
       LendingRow row;
       row.BookIsbn = query.value(query.record().indexOf(mBookIsbnColumn)).toString();
       row.BringBackDate = query.value(query.record().indexOf(mBringBackDateColumn)).toString();
       row.LendDate = query.value(query.record().indexOf(mLendDateColumn)).toString();
       row.ID = query.value(query.record().indexOf(mIdColumn)).toInt();
       row.GiveBackRow.ID = query.value(query.record().indexOf(mGiveBackIdColumn)).toInt();
       row.StudentBirthday = query.value(query.record().indexOf(mStudentBirthdayColumn)).toString();
       row.StudentForeName = query.value(query.record().indexOf(mStudentForNameColumn)).toString();
       row.StudentSurName = query.value(query.record().indexOf(mStudentSurNameColumn)).toString();
       row.type = static_cast<LendingType>(query.value(query.record().indexOf(mTypeColumn)).toInt());

       rows.append(row);
    }

    QSqlDatabase::database().close();
    return rows;
}

//----------------------
// getLendingsWithDamage: returns all lendingrows which have a returnrow with a registered damage
//---------------------
QList<LendingTable::LendingRow> LendingTable::getLendingsWithDamage()
{
    // Get the givebackrows with damage
    QList<GiveBackTable::GiveBackRow> rowList = mGiveBackTable.getRowsWithDamage();

    // Now get the lendingrows
    QList<LendingTable::LendingRow> returnList;
    for(GiveBackTable::GiveBackRow row : rowList)
    {
        returnList.append(*getLendingFromGiveBackId(row.ID));
    }

    return returnList;
}
