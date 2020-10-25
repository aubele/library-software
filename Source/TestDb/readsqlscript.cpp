#include "readsqlscript.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QDebug>


//--------------------
// ReadSqlScriptFile: Reads and parses the given sql-script-file and writes the data into the given database object.
// Parameter: db, fileName
//--------------------
int ReadSqlScriptFile(QSqlDatabase & db, const QString & fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return  0;

    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString sql = in.readAll();
    if (sql.length() == 0)
        return 0;

    QList<int> splitPoints;
    enum { IN_STR, IN_ESC, NORMAL } state = NORMAL;
    int successCount = 0;

    for (int i = 0; i < sql.length(); i++)
    {
        const int character = sql.at(i).unicode();
        switch (state)
        {
        case IN_STR:
            switch (character)
            {
            case '\'':
                state = NORMAL;
                break;
            case '\\':
                state = IN_ESC;
                break;
            }
            break;

        case IN_ESC:
            state = IN_STR;
            break;

        case NORMAL:
            switch (character)
            {
            case ';':
                splitPoints.push_back(i);
                break;

            case '\'':
                state = IN_STR;
                break;
            }
        }
    }

    splitPoints.push_back(sql.length() - 1);

    for (int i = 0, j = 0; i < splitPoints.length(); i++)
    {
        QString statement = sql.mid(j, splitPoints.at(i) - j + 1);
        j = splitPoints.at(i) + 1;

        if (statement.trimmed().length() > 0)
        {
            QSqlQuery query(db);
            if (query.exec(statement))
                successCount++;
            else
                qDebug() << "Failed:" << statement << "\nReason:" << query.lastError();
        }
    }

    return successCount;
}
