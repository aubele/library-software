#include "databasebackup.h"
#include "ui_databasebackup.h"
#include <QThread>
#include <QMutex>
#include <QDir>
#include <QDateTime>
#include <QFile>

//================================
// Implementation of Worker-Thread for DatabaseBackup
//================================
class DatabaseBackupWorker: public QThread
{
public:
    DatabaseBackupWorker(QString DatabaseNameWithPath, QString BackupFolderWithPath);
    DatabaseBackup::Error getError();

protected:
    void run();

private:
    QString mDatabaseNameWithPath;
    QString mBackupFolderWithPath;
    DatabaseBackup::Error mError;
    static QMutex mMutex;
};

QMutex DatabaseBackupWorker::mMutex;

DatabaseBackupWorker::DatabaseBackupWorker(QString DatabaseNameWithPath, QString BackupFolderWithPath) :
    mDatabaseNameWithPath(DatabaseNameWithPath), mBackupFolderWithPath(BackupFolderWithPath),
    mError(DatabaseBackup::NoError)
{
}

DatabaseBackup::Error DatabaseBackupWorker::getError()
{
    return mError;
}

void DatabaseBackupWorker::run()
{
    mMutex.lock();
    QDir dir;

    if (!dir.exists(mDatabaseNameWithPath))
    {
        mError = DatabaseBackup::SourceFileDoesNotExist;
        mMutex.unlock();
        return;
    }
    if (!dir.exists(mBackupFolderWithPath))
    {
        if (!dir.mkdir("Backup"))
        {
            mError = DatabaseBackup::DestinationFolderDoesNotExist;
            mMutex.unlock();
            return;
        }
    }

    int lastIndex = mDatabaseNameWithPath.lastIndexOf("/");
    if (lastIndex < 0)
        lastIndex = 0;
    else
        lastIndex++;

    QString databasename = mDatabaseNameWithPath.right(mDatabaseNameWithPath.length() - lastIndex);
    lastIndex = databasename.lastIndexOf(".");
    if (lastIndex <0)
        lastIndex = 0;
    QString ending = databasename.right(databasename.length()- lastIndex);

    databasename = databasename.replace(ending, "") + "-" + QDateTime::currentDateTime().
            toString("dd-MM-yyyy-hh-mm-ss") + ending;

    if(!QFile::copy(mDatabaseNameWithPath, mBackupFolderWithPath + "/" + databasename))
    {
        mError = DatabaseBackup::CopyError;
        mMutex.unlock();
        return;
    }

    mMutex.unlock();
}


//================================
// Implementation of class DatabaseBackup
//================================
//--------------------------------
// Constructor
//--------------------------------
DatabaseBackup::DatabaseBackup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseBackup)
{
    ui->setupUi(this);
}

//--------------------------------
// Destructor
//--------------------------------
DatabaseBackup::~DatabaseBackup()
{
    delete ui;
}

//--------------------------------
// makeBackup
// opens a wait dialog and runs the backup command in other thread
//--------------------------------
DatabaseBackup::Error DatabaseBackup::makeBackup(QString DatabaseNameWithPath, QString BackupFolderWithPath)
{
    DatabaseBackupWorker *worker = new DatabaseBackupWorker(DatabaseNameWithPath, BackupFolderWithPath);
    connect(worker, SIGNAL(finished()), this, SLOT(accept()));
    worker->start();
    this->exec();
    Error error = worker->getError();
    delete worker;
    return error;
}
