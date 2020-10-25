#ifndef DATABASERESTORATION_H
#define DATABASERESTORATION_H

#include <QDialog>
#include "db_global.h"

namespace Ui {
class DatabaseRestoration;
}

class DBSHARED_EXPORT DatabaseRestoration : public QDialog
{
    Q_OBJECT

public:
    explicit DatabaseRestoration( QString BackupFolderWithPath, QWidget *parent = 0);
    ~DatabaseRestoration();

private:
    Ui::DatabaseRestoration *ui;
    QString mBackupFolderWithPath;

private slots:
    void button_select_clicked();
    void button_start_clicked();
    void restoration_finished();
};

#endif // DATABASERESTORATION_H
