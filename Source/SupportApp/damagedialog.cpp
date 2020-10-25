#include "damagedialog.h"
#include "ui_damagedialog.h"


//================================
// Implementations of DamageDialog
//================================

//--------------------------------
// Constructor
//--------------------------------
DamageDialog::DamageDialog(QString booktitle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DamageDialog)
{
    ui->setupUi(this);
    ui->label_information->setText(ui->label_information->text().arg(booktitle));
    connect(ui->checkBox_damage, SIGNAL(stateChanged(int)), this, SLOT(checkbox_clicked(int)));
    connect(ui->pushButton_cancel, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->pushButton_OK, SIGNAL(clicked(bool)), this, SLOT(accept()));
    ui->pushButton_cancel->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_OK->setFocusPolicy(Qt::NoFocus);
}

//--------------------------------
// Destructor
//--------------------------------
DamageDialog::~DamageDialog()
{
    delete ui;
}

//--------------------------------
// getDamage(): returns the entered damage if it is available, otherwise ""
//--------------------------------
QString DamageDialog::getDamage()
{
    if(!ui->textEdit_damage->isEnabled())
        return "";
    return ui->textEdit_damage->toPlainText();
}

//--------------------------------
// checkbox_clicked(): enables the Textedit for damage if the Checkbox is selected
//--------------------------------
void DamageDialog::checkbox_clicked(int state)
{
    if(state == Qt::Checked)
        ui->textEdit_damage->setEnabled(true);
    else
        ui->textEdit_damage->setEnabled(false);
}
