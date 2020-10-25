#include "usertable.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QVariant>

//====================
// Implementation of UserTable
//====================

//--------------------
// Constructor
//--------------------
UserTable::UserTable(QString tablename, QString namecolumn, QString passwdcolumn, QString rolecolumn) noexcept :
    mTableName(tablename), mNameColumn(namecolumn), mPasswdColumn(passwdcolumn), mRoleColumn(rolecolumn)
{
}

//--------------------
// insert: adds a new user
// Parameter: UserRow containing a user
//--------------------
bool UserTable::insert(UserRow user) noexcept
{
    return insert(user.UserName, user.Passwd, user.Permissions);
}

//--------------------
// insert: adds a new user
// Parameter: username, passwd, permissions
//--------------------
bool UserTable::insert(QString username, QString passwd, UserPermissions permissions) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
        return false;

    QSqlQuery query;
    query.prepare("INSERT INTO " + mTableName + " (" + mNameColumn + ", " + mPasswdColumn + ", " + mRoleColumn + ") VALUES ('" + username + "', '" +
                  passwd + "', '" + QString::number(static_cast<int>(permissions)) + "');");
    if(query.exec())
           success = true;

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// remove: deletes a user from the database
// Parameter: username
//--------------------
bool UserTable::remove(QString username) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
        return false;

    QSqlQuery query;
    query.prepare("DELETE FROM " + mTableName + " WHERE " + mNameColumn + "='" + username + "';");
    if(query.exec())
           success = true;

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// getUser: finds a user in the database and returns it as a UserRow
// Parameter: username
//--------------------
UserTable::UserRow* UserTable::getUser(QString username)
{
    if(!QSqlDatabase::database().open())
        throw QString("Error in code: Method getUser of UserTable: Database could not be opened!");
    UserTable::UserRow* row = new UserTable::UserRow();
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mNameColumn + "='" + username + "';");
    int idName = query.record().indexOf(mNameColumn);
    int idPasswd = query.record().indexOf(mPasswdColumn);
    int idRole = query.record().indexOf(mRoleColumn);

    while (query.next())
    {
       row->UserName = query.value(idName).toString();
       row->Passwd = query.value(idPasswd).toString();
       row->Permissions = static_cast<UserPermissions>(query.value(idRole).toInt());
    }
    QSqlDatabase::database().close();
    return row;
}

//--------------------
// getUsers: finds all usesr with the given permissions and returns them as a list
// Parameter: permissions
//--------------------
QList<UserTable::UserRow> UserTable::getUsers(UserPermissions permissions)
{
    if(!QSqlDatabase::database().open())
        throw QString("Error in code: Method getUsers of UserTable: Database could not be opened!");
    QList<UserTable::UserRow> rows;
    QSqlQuery query("SELECT * FROM " + mTableName + " WHERE " + mRoleColumn + "='" + QString::number(static_cast<int>(permissions)) + "';");
    int idName = query.record().indexOf(mNameColumn);
    int idPasswd = query.record().indexOf(mPasswdColumn);
    int idRole = query.record().indexOf(mRoleColumn);

    while (query.next())
    {
       UserRow row;
       row.UserName = query.value(idName).toString();
       row.Passwd = query.value(idPasswd).toString();
       row.Permissions = static_cast<UserPermissions>(query.value(idRole).toInt());
       rows.append(row);
    }
    QSqlDatabase::database().close();
    return rows;
}

//--------------------
// getAllUsers: finds all users and returns them as a list
//--------------------
QList<UserTable::UserRow> UserTable::getAllUsers(UserTableSortOrder order)
{
    if(!QSqlDatabase::database().open())
		throw QString("Error in code: Method getAllUsers of UserTable: Database could not be opened!");
	QList<UserTable::UserRow> rows;
    QString sort = " ORDER BY ";
    switch(order)
    {
    case UserTable::UserNameAscending:
        sort += mNameColumn + " ASC";
        break;
    case UserTable::UserNameDescanding:
        sort += mNameColumn + "DESC";
        break;
    }
    QSqlQuery query("SELECT * FROM " + mTableName + sort + ";");
	int idName = query.record().indexOf(mNameColumn);
    int idPasswd = query.record().indexOf(mPasswdColumn);
    int idRole = query.record().indexOf(mRoleColumn);

	while (query.next())
    {
       UserRow row;
       row.UserName = query.value(idName).toString();
       row.Passwd = query.value(idPasswd).toString();
       row.Permissions = static_cast<UserPermissions>(query.value(idRole).toInt());
       rows.append(row);
    }
    QSqlDatabase::database().close();
    return rows;
}

//--------------------
// update: writes new values for the given username to the database
// Parameter: username, new passwd
//--------------------
bool UserTable::update(QString username, QString passwd) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
        return false;

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mPasswdColumn + " = '" + passwd + "' WHERE " + mNameColumn + " = '"+ username+ "';");

    if(query.exec())
           success = true;

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// update: writes new values for the given username to the database
// Parameter: username, new passwd, new permissions
//--------------------
bool UserTable::update(QString username, QString passwd, UserPermissions permissions) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
        return false;

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mPasswdColumn + " = '" + passwd + "', " + mRoleColumn
                  + " = " + QString::number(static_cast<int>(permissions)) + " WHERE " + mNameColumn + " = '"+ username+ "';");

    if(query.exec())
           success = true;

    QSqlDatabase::database().close();
    return success;
}

//--------------------
// update: writes new values for the given username to the database
// Parameter: username, new permissions
//--------------------
bool UserTable::update(QString username, UserPermissions permissions) noexcept
{
    bool success = false;
    if(!QSqlDatabase::database().open())
        return false;

    QSqlQuery query;
    query.prepare("UPDATE " + mTableName + " SET " + mRoleColumn + " = " + QString::number(static_cast<int>(permissions)) + " WHERE " + mNameColumn + " = '"+ username+ "';");

    if(query.exec())
           success = true;

    QSqlDatabase::database().close();
    return success;
}

//------------------
// Destructor
//------------------
UserTable::~UserTable()
{
}
