#include "usermanagementdialog.h"
#include "ui_usermanagementdialog.h"
#include "adduserdialog.h"

#include <QMessageBox>

//================================
// Implementations of UserManagementDialog
//================================

//--------------------------------
// Constructor
//--------------------------------
usermanagementdialog::usermanagementdialog(Dataset *set, QString mUserName, QWidget *parent) :
    mDataset(set),
    currentUser(mUserName),
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    ui(new Ui::usermanagementdialog)
{
    ui->setupUi(this);

    connect(ui->pushButton_close, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->pushButton_newUser, SIGNAL(clicked()), this, SLOT(addNewUser()));
    connect(ui->pushButton_deleteUser, SIGNAL(clicked()), this, SLOT(deleteUser()));
    connect(ui->pushButton_editUser, SIGNAL(clicked()), this, SLOT(editUser()));
    setupTableView();
}

//--------------------------------
// Destructor
//--------------------------------
usermanagementdialog::~usermanagementdialog()
{
    delete ui;
}

//--------------------------------
// setupTableView(): intialize the tableview for the usertable
//--------------------------------
void usermanagementdialog::setupTableView()
{
    // add model
    mModel = new UserModel(&mDataset->mUserTable, this);
    ui->tableView_User->setModel(mModel);
    // stretch
    ui->tableView_User->setHorizontalHeader(new QHeaderView(Qt::Horizontal, ui->tableView_User));
    ui->tableView_User->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // selection
    ui->tableView_User->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView_User->setSelectionBehavior(QAbstractItemView::SelectRows);

    mModel->fillModel();
}

//--------------------------------
// addNewUser(): calls the adduserdialog and adds the new user to the model and the database
//--------------------------------
void usermanagementdialog::addNewUser()
{
    adduserdialog dialog(mDataset, this);
    if(dialog.exec() == QDialog::Accepted)
    {
        mModel->addRow(dialog.getReturnUser());
        UserTable::UserRow *row = dialog.getReturnUser();
        mDataset->mUserTable.insert(*row);
        delete row;
    }
}

//--------------------------------
// deleteUser(): delets a user from the model and the database
//--------------------------------
void usermanagementdialog::deleteUser()
{
    QModelIndexList list = ui->tableView_User->selectionModel()->selectedRows();

    if (list.size() == 0)
        return;
    if(mModel->getRow(list.at(0).row())->UserName == currentUser)
    {
        // Setup messagebox
        QMessageBox msgBox;
        msgBox.setWindowTitle("Aktueller Benutzer");
        msgBox.setText("Dieser Benutzer ist aktuell angemeldet, daher kann er nicht gelöscht werden. Bitte melden Sie sich mit einem andern Benutzer, "
                       "mit administrativen Berechtigungen an und versuchen sie es erneut.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setButtonText(QMessageBox::Ok, "Ok");
        msgBox.exec();
        return;
    }
    // Setup messagebox
    QMessageBox msgBox;
    msgBox.setWindowTitle("Benutzer löschen");
    msgBox.setText("Wollen Sie den ausgewählten Benuter wirklich löschen?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setButtonText(QMessageBox::Cancel, "Abbrechen");
    int ret = msgBox.exec();


    if(ret == QMessageBox::Ok)
    {
        // Delete it
        int row = list.at(0).row();
        mModel->removeRows(row, 1);
    }
}

//--------------------------------
// editUser(): calls the adduserdialog with the data from the selected User and writes the changes to the model and the database
//--------------------------------
void usermanagementdialog::editUser()
{
    adduserdialog dialog(mDataset, this);
    QModelIndexList list = ui->tableView_User->selectionModel()->selectedRows();

    if (list.size() == 0)
        return;
    dialog.selectUser(currentUser, mModel->getRow(list.at(0).row()));
    if(dialog.exec() == QDialog::Accepted)
    {
        mModel->updateRow(dialog.getReturnUser(), list.at(0).row());
    }
}

//================================
// Implementations of UserModel
//================================

//--------------------------------
// operator==: set the == operator for two UserRows
//--------------------------------
bool operator==(UserTable::UserRow row1, const UserTable::UserRow row2)
{
    if (row1.UserName == row2.UserName)
        return true;
    else
        return false;
}

//--------------------------------
// Konstructor
//--------------------------------
UserModel::UserModel(UserTable *table, QObject *parent):
    QAbstractTableModel(parent), mUserTable(table)
{
}

//---------------------------------
// Destructor
//---------------------------------
UserModel::~UserModel()
{
    mUserTable = nullptr;
}

//---------------------------------
// headerData(): returns the value of for the header
//---------------------------------
QVariant UserModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch(section)
        {
        case 0:
            return "Benutzername";
        case 1:
            return "Berechtigungen";
        }
    }

    return QVariant();
}

//---------------------------------
// rowCount(): returns the number of rows
//---------------------------------
int UserModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mUsers.size();
}

//---------------------------------
// columnCount(): returns the number of columns
//---------------------------------
int UserModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

//---------------------------------
// data(): returns the value for the specified index and role
//---------------------------------
QVariant UserModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= mUsers.count())
        return QVariant();

    switch(role)
    {
        case Qt::DisplayRole:
        {
            UserTable::UserRow user = mUsers.at(index.row());
            switch(index.column())
            {
            case 0:
                return user.UserName;
            case 1:
            {
                QString permissions;
                int permission = user.Permissions;
                if(permission >= Administrative)
                {
                    permissions.append("Administrieren");
                    permission = permission - Administrative;
                    if(permission != 0)
                        permissions.append("; ");
                }
                if(permission >= Adding)
                {
                    permissions.append("Bearbeiten");
                    permission = permission - Adding;
                    if(permission != 0)
                        permissions.append("; ");
                }
                if(permission == Lending)
                {
                    permissions.append("Ausleihen");
                }
                return permissions;
            }
            }

        }
        case Qt::TextAlignmentRole:
        {
            return Qt::AlignCenter;
        }
        default:
            return QVariant();
    }
}

//---------------------------------
// removeRows(): removes a row at the given postion
//---------------------------------
bool UserModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (count > 1)
        return false;

    beginRemoveRows(parent, row, row);
    if(!mUserTable->remove(mUsers.at(row).UserName))
    {
        // if it fails just return
        endRemoveRows();
        return false;
    }
    // else fix the model
    mUsers.removeAt(row);
    endRemoveRows();
    return true;
}

//---------------------------
// reset(): clears the model so that new data can be set
//---------------------------
void UserModel::reset()
{
    beginResetModel();
    mUsers.clear();
    endResetModel();
}

//---------------------------
// fillModel(): fills the Model with the corresponding Data
//---------------------------
void UserModel::fillModel()
{
    beginResetModel();
    mUsers.clear();
    QList<UserTable::UserRow> srows;
    srows = mUserTable->getAllUsers();
    // copy the elements
    for(UserTable::UserRow srow : srows)
    {
        mUsers.append(srow);
    }
    endResetModel();
}

//---------------------------
// getRow(): returns the BookTable::BookRow for the given index
//---------------------------
UserTable::UserRow* UserModel::getRow(int row)
{
    return &(mUsers[row]);
}

//--------------------------
// updateRow(): updates the row at the given postion with the specified UserRow
//--------------------------
void UserModel::updateRow(UserTable::UserRow * updatedRow, int row)
{
    if(mUsers[row].UserName != updatedRow->UserName)
        return;
    else
    {
        mUsers[row].Passwd = updatedRow->Passwd;
        mUsers[row].Permissions = updatedRow->Permissions;
        emit dataChanged(this->index(row,0), this->index(row, 2));
    }
}

//--------------------------
// addRow(): adds a new row to the model
//--------------------------
void UserModel::addRow(UserTable::UserRow * newRow)
{
    emit beginInsertRows(QModelIndex(), rowCount(), rowCount());
    mUsers.append(*newRow);
    emit endInsertRows();
}


