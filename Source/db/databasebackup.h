#ifndef DATABASEBACKUP_H
#define DATABASEBACKUP_H

#include <QDialog>
#include "db_global.h"
#include <QString>

namespace Ui {
class DatabaseBackup;
}

class DBSHARED_EXPORT DatabaseBackup : private QDialog
{
    Q_OBJECT

public:
    enum Error {NoError, SourceFileDoesNotExist, DestinationFolderDoesNotExist, CopyError};
    explicit DatabaseBackup(QWidget *parent = 0);
    ~DatabaseBackup();
    Error makeBackup(QString DatabaseNameWithPath, QString BackupFolderWithPath);


private:
    Ui::DatabaseBackup *ui;
};

#endif // DATABASEBACKUP_H
