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
StudentModel::StudentModel(StudentTable* stable, LendingTable* ltable, QObject* parent) :
    QAbstractTableModel(parent), mStudentTable(stable), mLendigTable(ltable)
{
}

//--------------------------
// Destructor
//--------------------------
StudentModel::~StudentModel()
{
    mStudentTable = nullptr;
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
    return 5;
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

    if (index.column() >= 5)
        return QVariant();

    switch(role)
    {
        case Qt::DisplayRole:
        {
            StudentTable::StudentRow st = mStudents.at(index.row());
            // name in the second column
            if (index.column() == 1)
                return st.surName + ", " + st.preName;
            // date of birth in the third
            else if(index.column() == 2)
                return st.dateOfBirth;
            // classnames in the fourth
            else if(index.column() == 3)
            {
                QString allClasses;
                for(ClassTable::ClassRow c : st.classRows)
                {
                    allClasses.append(c.className + ", ");
                }
                allClasses.chop(2);
                return allClasses;
            }
            // amount of open lendings
            else if(index.column() == 4)
            {
                int size = mLendigTable->getOpenLendingsFromStudent(st.preName, st.surName, st.dateOfBirth).size();
                return size;
            }

        }
        case Qt::ToolTipRole:
        {
            // only the zero column will return a tooltip
            if (index.column() != 0)
                return QVariant();
            else if(!mStudents.at(index.row()).comment.isEmpty())
                return mStudents.at(index.row()).comment;
            else
                return QVariant();
        }
        case Qt::DecorationRole:
        {
            // only the zero column will return an icon
            if (index.column() != 0)
                return QVariant();
            else if(!mStudents.at(index.row()).comment.isEmpty())
                return QIcon(":/icons/icons/Statusinformation.png");
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
// headerData(): returns all information to display in the header
//---------------------------
QVariant StudentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        if(section == 1)
            return "Name";
        else if(section == 2)
            return "Geburtsdatum";
        else if(section == 3)
            return "Klassen";
        else if(section == 4)
            return "Anzahl an Ausleihen";
        else
            return QVariant();
    }
    else
        return QVariant();
}

//---------------------------
// selectClass(): queries information about a specified class
//---------------------------
void StudentModel::selectClass(QString classname)
{
    //if (mClass != classname)
    //{
        beginResetModel();
        mClass = classname;
        mStudents.clear();
        QList<StudentTable::StudentRow> srows;

        if(classname == "Alle")
            srows = mStudentTable->getAllStudents();
        else
            srows = mStudentTable->getStudentsByClassName(mClass);

        for(StudentTable::StudentRow srow : srows)
        {
            mStudents.append(srow);
        }
        endResetModel();
    //}
}

//---------------------------
// search(): searches for the given string
//---------------------------
void StudentModel::search(QString searchstringNameComment, QString searchstringAmount)
{
    // Return if both are empty
    if(searchstringNameComment.isEmpty() && searchstringAmount.isEmpty())
    {
        return;
    }

    beginResetModel();
    mStudents.clear();
    QList<StudentTable::StudentRow> srows;

    // Search for prename/surname/comment
    // Empty
    if(searchstringNameComment.isEmpty())
    {
        // Do nothing
    }
    else
    {
        srows = mStudentTable->searchStudentsByPreName(searchstringNameComment);
        QList<StudentTable::StudentRow> temprows;
        temprows = mStudentTable->searchStudentsBySurName(searchstringNameComment);
        // Check for duplicates
        for(StudentTable::StudentRow newRow : temprows)
        {
            if(!srows.contains(newRow))
            {
                srows.append(newRow);
            }
        }
        temprows = mStudentTable->searchStudentsByComment(searchstringNameComment);
        // Check for duplicates
        for(StudentTable::StudentRow newRow : temprows)
        {
            if(!srows.contains(newRow))
            {
                srows.append(newRow);
            }
        }
    }

    // Search for amount
    // Empty
    if(searchstringAmount.isEmpty())
    {
        // Do nothing
    }
    else if((searchstringAmount == '0' || searchstringAmount.toInt() != 0) && searchstringNameComment.isEmpty())
    {
        // pretty slow probably
        for(StudentTable::StudentRow studRow : mStudentTable->getAllStudents())
        {
            if(mLendigTable->getOpenLendingsFromStudent(studRow.preName, studRow.surName, studRow.dateOfBirth).size() == searchstringAmount.toInt())
            {
                srows.append((studRow));
            }
        }
    }
    else if((searchstringAmount == '0' || searchstringAmount.toInt() != 0) && !searchstringNameComment.isEmpty())
    {
        // pretty slow probably
        QList<StudentTable::StudentRow> srowsTemp = srows;
        srows.clear();
        for(StudentTable::StudentRow studRow : srowsTemp)
        {
            if(mLendigTable->getOpenLendingsFromStudent(studRow.preName, studRow.surName, studRow.dateOfBirth).size() == searchstringAmount.toInt())
            {
                srows.append((studRow));
            }
        }
    }

    // get them as students
    for(StudentTable::StudentRow srow : srows)
    {
        mStudents.append(srow);
    }
    endResetModel();
}

//---------------------------
// deleteStudent(): deletes the student from the given row
//---------------------------
void StudentModel::deleteStudent(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    // Get all lendigs from the student
    QList<LendingTable::LendingRow> lendList = mLendigTable->getLendingsFromStudent(mStudents.at(row).preName, mStudents.at(row).surName, mStudents.at(row).dateOfBirth);
    for(LendingTable::LendingRow lends : lendList)
    {
        // delete all lendings and givebacks from this student
        if(!mLendigTable->removeLending(lends.ID))
        {
            // if it fails just return
            endRemoveRows();
            return;
        }
    }
    // remove it from the db
    if(!mStudentTable->removeStudent(mStudents.at(row).preName, mStudents.at(row).surName, mStudents.at(row).dateOfBirth))
    {
        // if it fails just return
        endRemoveRows();
        return;
    }
    // fix the model
    mStudents.removeAt(row);
    endRemoveRows();
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
// getStudent returns the values of the current set student
//---------------------------
StudentTable::StudentRow* StudentModel::getStudent(QModelIndex index)
{
    return &(mStudents[index.row()]);
}


//---------------------------
// getStudent returns the student for the given row
//---------------------------
StudentTable::StudentRow* StudentModel::getStudent(int row)
{
    return &(mStudents[row]);
}

//---------------------------
// updateRow updates the model and its view
//---------------------------
void StudentModel::updateRow(int row)
{
    emit dataChanged(this->index(row,0), this->index(row, 3));
}

//---------------------------
// addStudent adds a new StudentRow to the model
//---------------------------
void StudentModel::addStudent(StudentTable::StudentRow* newRow)
{
    emit beginInsertRows(QModelIndex(), rowCount(QModelIndex()), rowCount(QModelIndex()));
    mStudents.append(*newRow);
    delete newRow;
    emit endInsertRows();
}

//=============================
// Implementations of ItemDelegateStudent
//=============================

//-----------------------------
// Konstruktor
//-----------------------------
ItemDelegateStudent::ItemDelegateStudent(QObject *parent)
        : QStyledItemDelegate(parent)
{
}

//------------------------------
// paint(): draws the items centered
//------------------------------
void ItemDelegateStudent::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QStyleOptionViewItemV4 viewItemOption(option);

    if (index.column() == 0)
    {
        QRect newRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
               QSize(option.decorationSize.width() + 5,option.decorationSize.height()),
               QRect(option.rect.x(), option.rect.y(),
               option.rect.width(), option.rect.height()));
        viewItemOption.rect = newRect;
    }
    QStyledItemDelegate::paint(painter, viewItemOption, index);
}
