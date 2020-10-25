#ifndef READSQLSCRIPT_H
#define READSQLSCRIPT_H

#include <QString>
#include <QSqlDatabase>

int ReadSqlScriptFile(QSqlDatabase & db, const QString & fileName);

#endif // READSQLSCRIPT_H
