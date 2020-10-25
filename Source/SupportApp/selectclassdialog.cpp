#include "selectClassDialog.h"
#include "ui_selectClassDialog.h"
#include "../baseqt/settingsdialog.h"
#include "../baseqt/configcache.h"
#include "../baseqt/aboutdialog.h"
#include "../db/classtable.h"
#include <selectclassdialog.h>
#include <classdialog.h>


//========================================
// Implementations of selectClassDialog
//========================================

//----------------------------------------
// Constructor
//----------------------------------------
selectClassDialog::selectClassDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::selectClassDialog)
{
    ui->setupUi(this);
    ui->pushButton_Settings->setFocusPolicy(Qt::NoFocus);
    ui->comboBox_classes->setFocusPolicy(Qt::NoFocus);
    connect(ui->pushButton_Settings, SIGNAL(clicked()), this, SLOT(openSettings()));
    QList<ClassTable::ClassRow> crows;
    crows = mDataSet.mClassTable.getAllClasses();
    for(ClassTable::ClassRow crow : crows)
    {
        mClasses.append(crow.className);
    }
    ui->comboBox_classes->addItems(mClasses);
    connect(ui->comboBox_classes, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(openPupils()));
    ui->pushButton_Settings->setFocusPolicy(Qt::NoFocus);
}

//----------------------------------------
// Destructor
//----------------------------------------
selectClassDialog::~selectClassDialog()
{
    delete ui;
}

//----------------------------------------
// openPupils(): opens the ClassDialog with all Pupils of the class
//----------------------------------------
void selectClassDialog::openPupils()
{
    QString classname = ui->comboBox_classes->currentText();
    ClassDialog dialog(&mDataSet, classname);
    dialog.showFullScreen();
    dialog.exec();
}

//----------------------------------------
// resetClasses(): clears the Classes, to reset the model
//----------------------------------------
void selectClassDialog::resetClasses()
{
    mClasses.clear();
}

//----------------------------------------
// openSettings(): opens the settingsDialog
//----------------------------------------
void selectClassDialog::openSettings()
{
//    SettingsDialog dialog(ConfigCache::getConfig("App.config"), this);
//    dialog.exec();
}


