#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "usertable.h"
#include "db_global.h"

namespace Ui {
class LoginDialog;
}

class DBSHARED_EXPORT LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(UserTable *usertable, QString lastuser, QWidget *parent = 0 );
    ~LoginDialog();
    QString getUserName();
    UserPermissions getUserRole();

private:
    Ui::LoginDialog *ui;
    UserTable* mUsertable;
    QString mUsername;
    UserPermissions mPermissions;

private slots:
    void button_login_clicked();

};

#endif // LOGINDIALOG_H
