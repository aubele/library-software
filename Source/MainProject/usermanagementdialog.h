#ifndef USERMANAGEMENTDIALOG_H
#define USERMANAGEMENTDIALOG_H

#include "usertable.h"
#include "dataset.h"
#include <QDialog>
#include <QAbstractTableModel>
#include <QVector>

class UserModel;

namespace Ui {
class usermanagementdialog;
}

class usermanagementdialog : public QDialog
{
    Q_OBJECT

public:
    explicit usermanagementdialog(Dataset *set, QString currentUser, QWidget *parent = 0);
    ~usermanagementdialog();


private:
    Ui::usermanagementdialog *ui;
    Dataset *mDataset;
    UserModel *mModel;
    QString currentUser;
    void setupTableView();

private slots:
    void addNewUser();
    void deleteUser();
    void editUser();
};

class UserModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit UserModel(UserTable *table, QObject *parent = nullptr);
    ~UserModel();

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void reset();
    void fillModel();

    UserTable::UserRow* getRow(int row);
    void updateRow(UserTable::UserRow * updatedRow, int row);

    void addRow(UserTable::UserRow * newRow);

private:
    UserTable *mUserTable;
    QVector<UserTable::UserRow> mUsers;
};
#endif // USERMANAGEMENTDIALOG_H
