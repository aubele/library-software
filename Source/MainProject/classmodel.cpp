#include "classmodel.h"
#include <QPainter>
#include <QCheckBox>

//=============================
// Implementations of StudentModel
//=============================

//--------------------------
// Constructor
//--------------------------
ClassModel::ClassModel(Dataset* set, QObject* parent) :
    QAbstractTableModel(parent),
    mDataset(set),
    mClassTable(&mDataset->mClassTable)
{
}

//--------------------------
// Destructor
//--------------------------
ClassModel::~ClassModel()
{
    mClassTable = nullptr;
}

//--------------------------
// rowCount(): returns the number of rows
//--------------------------
int ClassModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mClasses.count();
}

//---------------------------
// columnCount(): returns the number of columns
//---------------------------
int ClassModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

//---------------------------
// data(): which returns all informations to display
//---------------------------
QVariant ClassModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= mClasses.count())
        return QVariant();

    if (index.column() >= 4)
        return QVariant();

    switch(role)
    {
        case Qt::DisplayRole:
        {
            Class cl = mClasses.at(index.row());
            // classname in the second column
            if (index.column() == 1)
                return cl.className;
            // grade in the third
            else if(index.column() == 2)
                return cl.grade;
            // subject in the fourth
            else if(index.column() == 3)
                return cl.subject;
        }

        case Qt::CheckStateRole:
        {
            if(index.column() == 0)
            {
                bool selected = mClasses.at(index.row()).selected;
                if (selected)
                    return 1;
                else
                    return 0;
            }
            else
                return QVariant();
        }

        case Qt::TextAlignmentRole:
        {
            return Qt::AlignCenter;
        }

        default:
            return QVariant();

    }
}

//---------------------------
// setData sets the data of the specified item
//---------------------------
bool ClassModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    if(role != Qt::CheckStateRole)
        return false;

    if(index.column() == 0)
    {
        //set item checked
        if(value.toBool() == true)
        {
            mClasses[index.row()].selected = Qt::Checked;
            QVector<int> vect;
            vect.append(Qt::CheckStateRole);
            emit dataChanged(index, index, vect);
            return true;
        }
        //set item unchecked
        if(value.toBool() == false)
        {
            mClasses[index.row()].selected = Qt::Unchecked;
            QVector<int> vect;
            vect.append(Qt::CheckStateRole);
            emit dataChanged(index, index, vect);
            return true;
        }
        else
            return false;
    }
    return false;
}

//---------------------------
// headerData(): returns all information to display in the header
//---------------------------
QVariant ClassModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
            return QVariant();

        if (orientation == Qt::Horizontal)
        {
            if(section == 0)
                return "ausgewählt";
            if(section == 1)
                return "Klassenname";
            else if(section == 2)
                return "Klassenstufe";
            else if(section == 3)
                return "Fach";
            else
                return QVariant();
        }
        else
            return QVariant();
}

//-------------------------------------------
// flags returns the flags for each item
//-------------------------------------------
Qt::ItemFlags ClassModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    if (index.column() == 0)
        return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable;
    else
        return QAbstractTableModel::flags(index);
}

//--------------------------
// getSelectedClasses returns a List of all selected classes
//--------------------------
QList<ClassTable::ClassRow> ClassModel::getSelectedClasses()
{
    QList<ClassTable::ClassRow> selectedClasses;
    for(Class crow : mClasses)
    {
        if(crow.selected == true)
        {
            ClassTable::ClassRow row =
            {
                crow.className,
                crow.grade,
                crow.subject,
            };
            selectedClasses.append(row);
        }
     }
   return selectedClasses;
}

//---------------------------
// addClass(): add a new class to the Table
//---------------------------
void ClassModel::addClass(ClassTable::ClassRow newClass)
{
    emit beginInsertRows(QModelIndex(), rowCount(QModelIndex()), rowCount(QModelIndex()));
    Class mClass =
    {
        Qt::Checked,
        newClass.className,
        newClass.grade,
        newClass.subject
    };
    mClasses.append(mClass);
    emit endInsertRows();
}

//---------------------------
// selectClasses marks the specified classes as selected
//---------------------------
void ClassModel::selectClasses(QList<ClassTable::ClassRow> toSelectClasses)
{
    emit beginResetModel();
    for(ClassTable::ClassRow crow : toSelectClasses)
    {
        bool found = false;
        for (int i = 0; i < mClasses.length() && !found; i++)
        {
            if(crow.className == mClasses[i].className)
            {
                mClasses[i].selected = true;
                found = true;
            }
        }
        if (!found)
        {
            Class newClass = {
                Qt::Checked,
                crow.className,
                crow.grade,
                crow.subject
            };
            mClasses.append(newClass);
        }
    }
    emit endResetModel();
}

//---------------------------
// reset(): clears the model so that new data can be set
//---------------------------
void ClassModel::reset()
{
    beginResetModel();
    mClasses.clear();
    endResetModel();
}

//---------------------------
// fill(): fills the TableView with the Table Values
//---------------------------
void ClassModel::fill()
{
    beginResetModel();
    mClasses.clear();
    QList<ClassTable::ClassRow> crows;
    crows = mClassTable->getAllClasses();
    for(ClassTable::ClassRow crow : crows)
    {
        Class mClass =
        {
            Qt::Unchecked,
            crow.className,
            crow.grade,
            crow.subject
        };
        mClasses.append(mClass);
    }
    endResetModel();
}
