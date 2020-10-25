#include "logindialog.h"
#include "ui_logindialog.h"
#include <QCryptographicHash>

//------------------------------
// Konstructor
//------------------------------
LoginDialog::LoginDialog(UserTable *usertable, QString lastuser, QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    mUsertable = usertable;
    mPermissions = Nothing;
    ui->lineEdit_username->setText(lastuser);
    ui->label_error->hide();
}

//-----------------------------
// Destructor
//-----------------------------
LoginDialog::~LoginDialog()
{
    delete ui;
    mUsertable = nullptr;
}

//--------------------------------
// getUserName returns the selected username
//--------------------------------
QString LoginDialog::getUserName()
{
    return mUsername;
}

//--------------------------------
// getUserRole returns the permissions of the selected user
//--------------------------------
UserPermissions LoginDialog::getUserRole()
{
    return mPermissions;
}

//-------------------------------
// button_login_clicked: verfies the user with password
//-------------------------------
void LoginDialog::button_login_clicked()
{
    if (ui->lineEdit_username->text().isEmpty())
    {
        ui->lineEdit_username->setStyleSheet("background-color: rgb(255, 0, 0);");
        ui->lineEdit_username->setToolTip("Bitte geben Sie einen Benutzernamen ein");
        return;
    }
    else
    {
        ui->lineEdit_username->setToolTip("");
        ui->lineEdit_username->setStyleSheet("");
    }
    UserTable::UserRow* row = mUsertable->getUser(ui->lineEdit_username->text());
    QString hash = QString(QCryptographicHash::hash(ui->lineEdit_password->text().toUtf8(),QCryptographicHash::Md5).toHex());
    if (hash == row->Passwd && row->UserName == ui->lineEdit_username->text())
    {
        mUsername = ui->lineEdit_username->text();
        mPermissions = row->Permissions;
        this->accept();
    }
    else
        ui->label_error->show();
}
