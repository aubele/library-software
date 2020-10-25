#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QString programName, QString programVersion, QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->ProgramNameLabel->setText(programName);
    ui->ProgramVersionLabel->setText(ui->ProgramVersionLabel->text() + programVersion);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
