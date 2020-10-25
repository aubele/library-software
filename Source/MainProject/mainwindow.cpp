#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <settingsdialog.h>
#include <configcache.h>
#include <databasebackup.h>
#include <aboutdialog.h>
#include <excelimportdialog.h>
#include <logindialog.h>
#include <lizenzplugin.h>
#include <bookingwidget.h>
#include <studentwidget.h>
#include <databaserestoration.h>
#include <QDir>

#include <QMessageBox>
#include <QPushButton>
#include "bookwidget.h"
#include "usermanagementdialog.h"
#include "OpenLendings.h"
#include "DamagesWidget.h"
#include "coursesubjectcorrectiondialog.h"
#include "appwidget.h"

QString getTextForError(DatabaseBackup::Error error)
{
    QString text;

    switch(error)
    {
    case DatabaseBackup::CopyError:
        text = "Die Datenbank konnte nicht kopiert werden! Existiert die Datenbank?";
        break;
    case DatabaseBackup::DestinationFolderDoesNotExist:
        text = "Backup ist fehlgeschlagen! Der Backup-Ordner konnte nicht erstellt werden.";
        break;
    case DatabaseBackup::SourceFileDoesNotExist:
        text = "Backup ist fehlgeschlagen! Die Datenbankdatei konnte nicht gefunden werden.";
        break;
    default:
        text = "Backup erfolgreich.";
    }

    return text;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mIsInitialized = false;
    mLastAction = nullptr;
    mPermissions = Nothing;
    Licenceplugin::CreatePlugin();
    Licenceplugin::Add3rdPartyLicence(new LicenceItem());

    connect(ui->actionSchueler, SIGNAL(triggered()), this, SLOT(rider_students_triggered()));
    connect(ui->actionMedien, SIGNAL(triggered()), this, SLOT(rider_books_triggered()));
    connect(ui->actionApp, SIGNAL(triggered()), this, SLOT(rider_Unterstuetzungsapp_triggered()));
    // hide the context menu
    setContextMenuPolicy(Qt::NoContextMenu);

    styleWindow();
}

//------------------------------------
// styleWindow styles the window based on the cfg
//------------------------------------
void MainWindow::styleWindow()
{
    /*Config *cfg = ConfigCache::getConfig("liborg.config");
    if (cfg)
    {
        QColor brightcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Dark", VariousValue(VariousValue::RGB_Color, "177,216,228")).asColor();
        QColor darkcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Light", VariousValue(VariousValue::RGB_Color, "240,253,253")).asColor();

    }*/
}

MainWindow::~MainWindow()
{
    delete ui;
}

//------------------------------------------
// Programm -> Einstellungen
//------------------------------------------
void MainWindow::rider_settings_triggered()
{
    SettingsDialog dialog(ConfigCache::getConfig("liborg.config"), this);
    dialog.exec();
}

//-----------------------------------------
// Hilfe -> Über
//-----------------------------------------
void MainWindow::rider_about_triggered()
{
    AboutDialog dialog("LibOrg - The Library Organizer", "1.0", this);
    dialog.exec();
}

//-----------------------------------------
// Hilfe -> 3rd Party Software
//-----------------------------------------
void MainWindow::rider_3rd_party_software_triggered()
{
    Licenceplugin::ShowLicences(this);
}

//----------------------------------------
// Programm -> Import
//----------------------------------------
void MainWindow::rider_import_triggered()
{
    clearWidget();
    ExcelImportDialog dialog(&(mDataSet.mStudentTable), &(mDataSet.mLendTable), this);
    dialog.exec();
}

//-----------------------------------------
// Programm -> Benutzer wechseln
//-----------------------------------------
void MainWindow::rider_changeuser_triggered()
{
    UserPermissions  permissions = Nothing;
    // open login dialog
    QString lastuser = ConfigCache::getConfig("liborg.config")->getValueForKeyInBlock("Login", "LastUser", VariousValue(VariousValue::Text, "")).asText();
    LoginDialog dialog(&(mDataSet.mUserTable), lastuser, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        clearWidget();
        mUserName = dialog.getUserName();
        permissions = dialog.getUserRole();
        Config *cfg = ConfigCache::getConfig("liborg.config");
        cfg ->setValueForKeyInBlock("Login", "LastUser", mUserName);
        cfg->save();
        initUiWithPermissions(permissions);
    }
}

//-----------------------------------------
// Programm -> Benutzerverwaltung
//-----------------------------------------
void MainWindow::rider_usermanagement_triggerd()
{
    //clearWidget();
    usermanagementdialog dialog(&(mDataSet), mUserName, this);
    dialog.exec();
}


//----------------------------------------
// Programm -> before close
//----------------------------------------
void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox box(QMessageBox::Question, "Beenden?", "Wollen Sie das Programm wirklich beenden?",
                        QMessageBox::NoButton, this );
    QPushButton *yes = box.addButton(QString("Ja"), QMessageBox::YesRole);
    box.addButton(QString("Nein"), QMessageBox::NoRole);

    box.exec();
    if (box.clickedButton() == yes)
    {
        bool shouldBackup = ConfigCache::getConfig("liborg.config")->getValueForKeyInBlock("Backup",
                "BackupOnSystemEnd", VariousValue(VariousValue::Bool, "1")).asBool();

        if (shouldBackup)
        {
            DatabaseBackup backup(this);
            DatabaseBackup::Error error = backup.makeBackup("database.db", "Backup");
            if (error != DatabaseBackup::NoError)
            {
                QMessageBox box(QMessageBox::Information, "Information", getTextForError(error),
                                QMessageBox::Ok, this);
                box.exec();
            }
        }
    }
    else
        event->ignore();
}

//-----------------------------------------
// Programm -> Fenster geladen
//-----------------------------------------
void MainWindow::window_loaded()
{
    bool shouldBackup = ConfigCache::getConfig("liborg.config")->getValueForKeyInBlock("Backup",
            "BackupOnStartup", VariousValue(VariousValue::Bool, "1")).asBool();

    if (shouldBackup)
    {
        DatabaseBackup backup(this);
        DatabaseBackup::Error error = backup.makeBackup("database.db", "Backup");
        if (error != DatabaseBackup::NoError)
        {
            QMessageBox box(QMessageBox::Information, "Information", getTextForError(error),
                   QMessageBox::Ok, this);
            box.exec();
        }
    }

    UserPermissions  permissions = Nothing;
    // open login dialog
    QString lastuser = ConfigCache::getConfig("liborg.config")->getValueForKeyInBlock("Login", "LastUser", VariousValue(VariousValue::Text, "")).asText();
    LoginDialog dialog(&(mDataSet.mUserTable), lastuser, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        mUserName = dialog.getUserName();
        permissions = dialog.getUserRole();
        Config *cfg = ConfigCache::getConfig("liborg.config");
        cfg ->setValueForKeyInBlock("Login", "LastUser", mUserName);
        cfg->save();
    }
    initUiWithPermissions(permissions);
}

//------------------------------------
// paintEvent calls QMainWindow::paintEvent but is required to detect if the window is loaded
//------------------------------------
void MainWindow::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);

    if (mIsInitialized)
        return;
    mIsInitialized = true;
    window_loaded();
}

//---------------------------------------
// initUiWithPermissions: all actions are enabled with are allowed for the given permissions
//---------------------------------------
void MainWindow::initUiWithPermissions (UserPermissions permissions)
{
    mPermissions = permissions;

    //first say that the user has absolutely no rights
    ui->actionAusleihe_und_Rueckgabe->setEnabled(false);
    ui->actionEinstellungen->setEnabled(false);
    ui->actionImport->setEnabled(false);
    ui->actionMedien->setEnabled(false);
    ui->actionSchueler->setEnabled(false);
    ui->actionWiederherstellung->setEnabled(false);
    ui->actionBenutzerverwaltung->setEnabled(false);
    ui->actionOffene_Ausleihen->setEnabled(false);
    ui->actionDamages->setEnabled(false);
    ui->actionApp->setEnabled(false);

    if (permissions & Lending)
    {
        ui->actionAusleihe_und_Rueckgabe->setEnabled(true);
        ui->actionSchueler->setEnabled(true);
        ui->actionOffene_Ausleihen->setEnabled(true);
        ui->actionDamages->setEnabled(true);
        ui->actionApp->setEnabled(true);
    }
    if (permissions & Adding)
    {
        ui->actionMedien->setEnabled(true);
        ui->actionImport->setEnabled(true);
    }
    if (permissions & Administrative)
    {
        ui->actionEinstellungen->setEnabled(true);
        ui->actionWiederherstellung->setEnabled(true);
        ui->actionBenutzerverwaltung->setEnabled(true);
    }
}

//----------------------------------------
// rider_lend_and_giveback_triggered() called by the rider "Ausleihe und Rückgabe"
//----------------------------------------
void MainWindow::rider_lend_and_giveback_triggered()
{
    // first clear the widget
    clearWidget();
    // then add the bookingwidget
    BookingWidget *widget = new BookingWidget(&mDataSet, &mLastClass,  ui->widget);
    mLastAction = ui->actionAusleihe_und_Rueckgabe;
    mLastAction->setEnabled(false);
    connect(widget, SIGNAL(closewidget()), this, SLOT(clearWidget()));
    QVBoxLayout *layout = new QVBoxLayout(ui->widget);
    layout->addWidget(widget);
    layout->setContentsMargins(0,0,0,0);
    ui->widget->setLayout(layout);
}


//----------------------------------------
// rider_students_triggered() called by the rider "Schüler"
//----------------------------------------
void MainWindow::rider_students_triggered()
{
    // first clear the widget
    clearWidget();
    // add studentwidget
    StudentWidget* widget = new StudentWidget(&mDataSet, mPermissions, &mLastClass, ui->widget);
    mLastAction = ui->actionSchueler;
    mLastAction->setEnabled(false);
    connect(widget, SIGNAL(closewidget()), this, SLOT(clearWidget()));
    QVBoxLayout *layout = new QVBoxLayout(ui->widget);
    layout->addWidget(widget);
    layout->setContentsMargins(0,0,0,0);
    ui->widget->setLayout(layout);
}

//---------------------------------------
// rider_books_triggered() called by the rider "Bücher"
//---------------------------------------
void MainWindow::rider_books_triggered()
{
    // first clear the widget
    clearWidget();
    // add bookwidget
    BookWidget* widget = new BookWidget(&mDataSet, ui->widget);
    mLastAction = ui->actionMedien;
    mLastAction->setEnabled(false);
    connect(widget, SIGNAL(closewidget()), this, SLOT(clearWidget()));
    QVBoxLayout *layout = new QVBoxLayout(ui->widget);
    layout->addWidget(widget);
    layout->setContentsMargins(0,0,0,0);
    ui->widget->setLayout(layout);
}

//----------------------------------------
// clearWidget removes all childs from the widget
//----------------------------------------
void MainWindow::clearWidget()
{
    if (mLastAction)
        mLastAction->setEnabled(true);
    QObjectList childWidgets = ui->widget->children();
    foreach(QObject* widget, childWidgets)
    {
        ui->widget->layout()->removeWidget(static_cast<QWidget*>(widget));
        delete widget;
    }
    mLastAction = nullptr;
}

//------------------------------------------
// slot: rider_restoration_triggered called by the restoration action of settings
//------------------------------------------
void MainWindow::rider_restoration_triggered()
{
    DatabaseRestoration dialog(QDir::currentPath() + "/Backup", this);
    clearWidget();
    dialog.exec();
}

//----------------------------------------
// slot: rider_open_lendings_triggered called by openlendings
//----------------------------------------
void MainWindow::rider_open_lendings_triggered()
{
    // first clear the widget
    clearWidget();
    // add OpenLendings widget
    OpenLendings* widget = new OpenLendings(&mDataSet, ui->widget);
    mLastAction = ui->actionOffene_Ausleihen;
    mLastAction->setEnabled(false);
    connect(widget, SIGNAL(closewidget()), this, SLOT(clearWidget()));
    QVBoxLayout *layout = new QVBoxLayout(ui->widget);
    layout->addWidget(widget);
    layout->setContentsMargins(0,0,0,0);
    ui->widget->setLayout(layout);
}

//----------------------------------------
// rider_damages_triggered() called by the action damages
//----------------------------------------
void MainWindow::rider_damages_triggered()
{
    // first clear the widget
    clearWidget();
    // TODO: correct widget creation
    DamagesWidget* widget = new DamagesWidget(&mDataSet, ui->widget);
    mLastAction = ui->actionDamages;
    mLastAction->setEnabled(false);
    connect(widget, SIGNAL(closewidget()), this, SLOT(clearWidget()));
    QVBoxLayout *layout = new QVBoxLayout(ui->widget);
    layout->addWidget(widget);
    layout->setContentsMargins(0,0,0,0);
    ui->widget->setLayout(layout);
}

//----------------------------------------
// rider_Kurs_Fach_Zuordnung_triggered() called by the action Kurs-Fach-Zuordnung berichtigen triggered
//----------------------------------------
void MainWindow::rider_Kurs_Fach_Zuordnung_triggered()
{
    CourseSubjectCorrectionDialog dialog(&mDataSet.mClassTable, this);
    clearWidget();
    dialog.exec();
}

//----------------------------------------
// rider_Unterstuetzungsapp_triggered() called by the action Unterstützungsapp
//----------------------------------------
void MainWindow::rider_Unterstuetzungsapp_triggered()
{
    clearWidget();
    AppWidget* widget = new AppWidget(ui->widget);
    mLastAction = ui->actionApp;
    mLastAction->setEnabled(false);
    connect(widget, SIGNAL(closewidget()), this, SLOT(clearWidget()));
    QVBoxLayout *layout = new QVBoxLayout(ui->widget);
    layout->addWidget(widget);
    layout->setContentsMargins(0,0,0,0);
    ui->widget->setLayout(layout);
}
