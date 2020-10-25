#include "abstractdatabase.h"
#include <QSqlDatabase>

//====================================
// Implementation of class AbstractDatabase
//====================================

//------------------------------------
// Constructor
//------------------------------------
AbstractDatabase::AbstractDatabase(QString drivername, QString dbname, QString username, QString password)
{
    QSqlDatabase db = QSqlDatabase::addDatabase(drivername);
    db.setDatabaseName(dbname);
    if (!username.isEmpty() && !password.isEmpty())
    {
        db.setUserName(username);
        db.setPassword(password);
    }
}

//------------------------------------
// Destructor
//------------------------------------
AbstractDatabase::~AbstractDatabase()
{}
