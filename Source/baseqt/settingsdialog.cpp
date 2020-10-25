#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QMessageBox>
#include <QCloseEvent>
#include "configcache.h"

//=======================================
// Implementtion of class SettingsDialog
//=======================================

//---------------------------------------
// Constructor
//---------------------------------------
SettingsDialog::SettingsDialog(Config* cfg, QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    ui(new Ui::SettingsDialog),  mCfg(cfg)
{
    ui->setupUi(this);
    ui->saveButton->setEnabled(false);
    mIsModified = false;
    ui->scrollArea->setWidget(new ConfigWidget(cfg, this, parent));
}

//---------------------------------------
// Destructor
//---------------------------------------
SettingsDialog::~SettingsDialog()
{
    delete ui;
    mCfg = nullptr;
}

//----------------------------------------
// setModified: enables or disables the savebutton
//----------------------------------------
void SettingsDialog::setModified(bool modified)
{
    ui->saveButton->setEnabled(modified);

    if (modified && !mIsModified)
        setWindowTitle(windowTitle() + "*" );
    else if(!modified)
        setWindowTitle(windowTitle().replace("*", ""));

    mIsModified = modified;
}

//----------------------------------------
// closeEvent: test if the dialog must be saved
//----------------------------------------
void SettingsDialog::closeEvent(QCloseEvent *event)
{
    if (mIsModified)
    {
        QMessageBox msgBox;
        msgBox.setText("Die Konfiguration wurde verändert.");
        msgBox.setInformativeText("Möchten Sie die Änderungen speichern?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        msgBox.setButtonText(QMessageBox::Save, "Speichern");
        msgBox.setButtonText(QMessageBox::Discard, "Verwerfen");
        msgBox.setButtonText(QMessageBox::Cancel, "Abbrechen");
        int ret = msgBox.exec();
        switch (ret)
        {
            case QMessageBox::Save:
            {
                mCfg->save();
                break;
            }
            case QMessageBox::Cancel:
            {
                mCfg->load();
                event->ignore();                
                break;
            }
        }
    }
}

//-----------------------------------
// save(): speichert die Konfiguration
//-----------------------------------
void SettingsDialog::save()
{
    mCfg->save();
    setModified(false);
}
