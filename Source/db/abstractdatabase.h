#ifndef ABSTRACTDATABASE_H
#define ABSTRACTDATABASE_H
#include "db_global.h"
#include <QString>

class DBSHARED_EXPORT AbstractDatabase
{
public:
    AbstractDatabase(QString drivername, QString dbname, QString username=QString(), QString password=QString());
    virtual ~AbstractDatabase();
};

#endif // ABSTRACTDATABASE_H
