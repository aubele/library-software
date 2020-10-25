#include "databaserestoration.h"
#include "ui_databaserestoration.h"
#include <QFileDialog>
#include <QThread>

//==================================
// Definition of private RestorationThread
//==================================
class RestorationThread : public QThread
{
public:
    RestorationThread(QString filenameWithPath);
    ~RestorationThread();

private:
    QString mFilename;

protected:
    void run();
};

//----------------------------------
// Konstructor
//----------------------------------
RestorationThread::RestorationThread(QString filenameWithPath) :
    mFilename(filenameWithPath)
{
}

//----------------------------------
// Destructor
//----------------------------------
RestorationThread::~RestorationThread()
{}

//----------------------------------
// Process thread: run()
// restores file
//----------------------------------
void RestorationThread::run()
{
    // remove old db file
    QDir current = QDir::current();
    current.remove("database.db");
    QFile::copy(mFilename, "database.db");
}

//==================================
// Definitions of DatabaseRestoration
//==================================

//----------------------------------
// Konstructor
//----------------------------------
DatabaseRestoration::DatabaseRestoration(QString BackupFolderWithPath, QWidget *parent ) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    ui(new Ui::DatabaseRestoration), mBackupFolderWithPath(BackupFolderWithPath)
{
    ui->setupUi(this);
    ui->button_start->setEnabled(false);
    ui->widget_progress->hide();
    connect(ui->button_select, SIGNAL(clicked()), this, SLOT(button_select_clicked()));
    connect(ui->button_start, SIGNAL(clicked()), this, SLOT(button_start_clicked()));
}

//------------------------------------
// Destructor
//------------------------------------
DatabaseRestoration::~DatabaseRestoration()
{
    delete ui;
}

//--------------------------------------
// Slot: button_select_clicked called by button_select
// opens a QFileDialog to select the file name (*.db) for the file to be restored
//--------------------------------------
void DatabaseRestoration::button_select_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Datei zur Wiederherstellung wählen", mBackupFolderWithPath,"*.db");

    if (!filename.isNull())
    {
        ui->button_start->setEnabled(true);
        ui->label_datei->setText(filename);
    }
}

//--------------------------------------
// Slot: button_start_clicked called by button_start
// start the restoration
//--------------------------------------
void DatabaseRestoration::button_start_clicked()
{
    ui->button_start->setEnabled(false);
    ui->widget_progress->show();
    ui->label_restoration_complete->hide();
    ui->button_close->setEnabled(false);
    RestorationThread* thread = new RestorationThread(ui->label_datei->text());
    connect(thread, SIGNAL(finished()), this, SLOT(restoration_finished()));
    thread->start();
}

//-------------------------------------
void DatabaseRestoration::restoration_finished()
{
    ui->label_restoration_complete->show();
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(100);
    ui->button_close->setEnabled(true);
    ui->button_start->setEnabled(false);
}
