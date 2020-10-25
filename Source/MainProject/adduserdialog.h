#ifndef ADDUSERDIALOG_H
#define ADDUSERDIALOG_H

#include "usertable.h"
#include "dataset.h"
#include <QDialog>

namespace Ui {
class adduserdialog;
}

class adduserdialog : public QDialog
{
    Q_OBJECT

public:
    explicit adduserdialog(Dataset* set, QWidget *parent = 0);
    ~adduserdialog();
    void selectUser(QString user, UserTable::UserRow* row);
    UserTable::UserRow* getReturnUser();

private:
    Ui::adduserdialog *ui;
    Dataset* mDataset;
    UserTable::UserRow *mReturnUser;
    bool mModify;
private slots:
    void apply();
};

#endif // ADDUSERDIALOG_H
