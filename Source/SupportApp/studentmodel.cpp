#include "studentmodel.h"
#include "QIcon"
#include <QPainter>
#include "QDebug"


//=============================
// Implementations of StudentModel
//=============================

//--------------------------
// Constructor
//--------------------------
StudentModel::StudentModel(QList<StudentTable::StudentRow> students, LendingTable* ltable, QObject* parent) :
    QAbstractTableModel(parent), mStudents(students), mLendigTable(ltable)
{
}

//--------------------------
// Destructor
//--------------------------
StudentModel::~StudentModel()
{
}

//--------------------------
// rowCount(): returns the number of rows
//--------------------------
int StudentModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mStudents.count();
}

//---------------------------
// columnCount(): returns the number of columns
//---------------------------
int StudentModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

//---------------------------
// data(): which returns all informations to display
//---------------------------
QVariant StudentModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= mStudents.count())
        return QVariant();

    if (index.column() >= 2)
        return QVariant();

    switch(role)
    {
        case Qt::DisplayRole:
        {
            StudentTable::StudentRow st = mStudents.at(index.row());
            // name in the first column
            if (index.column() == 0)
                return st.surName + ", " + st.preName;
            // amount of open lendings
            else if(index.column() == 1)
            {
                int size = mLendigTable->getOpenLendingsFromStudent(st.preName, st.surName, st.dateOfBirth).size();
                return size;
            }

        }
//        case Qt::TextAlignmentRole:
//        {
//            return Qt::AlignCenter;
//        }
        default:
            return QVariant();
       }
}

//---------------------------
// headerData(): returns all information to display in the header
//---------------------------
QVariant StudentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        if(section == 0)
            return "Name";
        else if(section == 1)
            return "Ausleihen";
        else
            return QVariant();
    }
    else
        return QVariant();
}

//---------------------------
// reset(): clears the model so that new data can be set
//---------------------------
void StudentModel::reset()
{
    beginResetModel();
    mStudents.clear();
    endResetModel();
}

//---------------------------
// getStudent(): returns the values of the current set student
//---------------------------
StudentTable::StudentRow* StudentModel::getStudent(QModelIndex index)
{
    return &(mStudents[index.row()]);
}


//---------------------------
// getStudent(): returns the student for the given row
//---------------------------
StudentTable::StudentRow* StudentModel::getStudent(int row)
{
    return &(mStudents[row]);
}

//---------------------------
// updateRow(): updates the model and its view
//---------------------------
void StudentModel::updateRow(int row)
{
    emit dataChanged(this->index(row,0), this->index(row, 3));
}
