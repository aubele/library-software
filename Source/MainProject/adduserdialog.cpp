#include "adduserdialog.h"
#include "ui_adduserdialog.h"

#include <QCryptographicHash>
#include <QRegExpValidator>
#include <QMessageBox>


//================================
// Implementations of addUserDialog
//================================

//--------------------------------
// Constructor
//--------------------------------
adduserdialog::adduserdialog(Dataset* set, QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    mDataset(set),
    ui(new Ui::adduserdialog),
    mReturnUser(nullptr)

{
    ui->setupUi(this);

    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->pushButton_addUser, SIGNAL(clicked()), this, SLOT(apply()));

    //Regular Expressions
    QRegExp rxName("^[a-zA-ZöÖäÄüÜß\\-]{1,50}$");
    QRegExp rxPassword("^[\\S]{5,50}$");
    //Validators for the Expressions
    QValidator *validatorName = new QRegExpValidator(rxName, this);
    QValidator *validatorPassword = new QRegExpValidator(rxPassword, this);
    //connect the Validators and the lineEdits
    ui->lineEdit_UserName->setValidator(validatorName);
    ui->lineEdit_Password->setValidator(validatorPassword);
}

//--------------------------------
// Destructor
//--------------------------------
adduserdialog::~adduserdialog()
{
    delete ui;
    mReturnUser = nullptr;
}


//-----------------------------
// selectUser(): places the given values in the ui
//-----------------------------
void adduserdialog::selectUser(QString currentUser, UserTable::UserRow* row)
{
    mModify = true;
    mReturnUser = row;
    ui->lineEdit_UserName->setText(row->UserName);
    ui->lineEdit_UserName->setDisabled(true);
    ui->label_Password->setText("Neues Passwort (wenn leer wird das alte beibehalten)");
    int permission = (int)row->Permissions;
    if(permission >= Administrative)
    {
        ui->checkBox_Administrative->setChecked(true);
        permission = permission - Administrative;
    }
    if(permission >= Adding)
    {
        ui->checkBox_Edit->setChecked(true);
        permission = permission - Adding;
    }
    if(permission == Lending)
    {
        ui->checkBox_Lending->setChecked(true);
    }
    if(row->UserName == currentUser)
    {
        ui->checkBox_Administrative->setDisabled(true);
        ui->checkBox_Edit->setDisabled(true);
        ui->checkBox_Lending->setDisabled(true);
        ui->label_Permissions->setText("Berechtigungen - um diese zu ändern loggen Sie sich bitte, mit einem anderen administrativen Benutzer ein");
    }
    ui->pushButton_addUser->setText("Übernehmen");
}


//-----------------------------
// apply(): applys the inserted data and/or changes to the model and the database
//-----------------------------
void adduserdialog::apply()
{
    QString userName = ui->lineEdit_UserName->text();
    bool existing = false;
    for(UserTable::UserRow user : mDataset->mUserTable.getAllUsers())
    {
        if(user.UserName == userName)
            existing = true;
    }
    if(existing && mModify != true)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Benutzer vorhanden");
        msgBox.setText("Der angegebene Benutzer ist bereits vorhanden. Bitte ändern sie den Benutzernamen und versuchen sie es erneut.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setButtonText(QMessageBox::Ok, "Ok");
        msgBox.exec();
    }
    else
    {
        QString password = ui->lineEdit_Password->text();
        QString passwordHash = QString(QCryptographicHash::hash(ui->lineEdit_Password->text().toUtf8(), QCryptographicHash::Md5).toHex());
        UserPermissions permission = Nothing;
        if(ui->checkBox_Lending->checkState())
            permission = permission | Lending;
        if(ui->checkBox_Edit->checkState())
            permission = permission | Adding;
        if(ui->checkBox_Administrative->checkState())
            permission = permission | Administrative;

        if(mModify)
        {
            mReturnUser = new UserTable::UserRow;
            mReturnUser->UserName = userName;
            mReturnUser->Passwd = passwordHash;
            mReturnUser->Permissions = permission;
            if(password == "")
            {
                mDataset->mUserTable.update(userName, permission);
                accept();
            }
            else
            {
                mDataset->mUserTable.update(userName, passwordHash, permission);
                accept();
            }
        }
        else
        {
            mReturnUser = new UserTable::UserRow;
            mReturnUser->UserName = userName;
            mReturnUser->Passwd = passwordHash;
            mReturnUser->Permissions = permission;
            accept();
        }
    }
}


//-----------------------------
// getReturnUser(): returns the new user from the dialog
//-----------------------------
UserTable::UserRow* adduserdialog::getReturnUser()
{
    return mReturnUser;
}
