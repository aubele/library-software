#include "bookmodel.h"
#include <QMessageBox>

//================================
// Implementations of BookModel
//================================
bool operator==(BookTable::BookRow row1, const BookTable::BookRow row2)
{
    if (row1.isbn == row2.isbn)
        return true;
    else
        return false;
}

//--------------------------------
// Konstructor
//--------------------------------
BookModel::BookModel(BookTable *table, LendingTable *ltable, QObject *parent)
    : QAbstractTableModel(parent), mBookTable(table), mLendTable(ltable)
{
}

//---------------------------------
// Destructor
//---------------------------------
BookModel::~BookModel()
{
    mBookTable = nullptr;
    mLendTable = nullptr;
}

//---------------------------------
// headerData returns the value of a header
//---------------------------------
QVariant BookModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch(section)
        {
        case 0:
            return "Titel";
        case 1:
            return "Untertitel";
        case 2:
            return "ISBN";
        case 3:
            return "Jahrgangsstufe";
        case 4:
            return "Fach";
        case 5:
            return "Verfügbar";
        }
    }

    return QVariant();
}

//---------------------------------
// rowCount returns the number of rows
//---------------------------------
int BookModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mBooks.size();
}

//---------------------------------
// columnCount returns the number of columns
//---------------------------------
int BookModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 6;
}

//---------------------------------
// data removes the value for the specified index and role
//---------------------------------
QVariant BookModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= mBooks.count())
        return QVariant();

    switch(role)
    {
        case Qt::DisplayRole:
        {
            BookTable::BookRow st = mBooks.at(index.row());
            switch(index.column())
            {
            case 0:
                return st.title;
            case 1:
                return st.subtitle;
            case 2:
                return st.isbn;
            case 3:
            {
                int value;
                QString returnValue;
                foreach (value, st.grade)
                    returnValue += QString::number(value) + ";";
                return returnValue;
            }
            case 4:
                return st.subject;
            case 5:
                return st.available;
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
// removeRows removes a row
//---------------------------------
bool BookModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (count > 1)
        return false;

    if(mLendTable->getLendingsFromBook(mBooks.at(row).isbn).length() != 0)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Buch kann nicht gelöscht werden");
        msgBox.setText("Ein oder mehrere Exemplare des Buches sind noch ausgeliehen. Buch kann nicht gelöscht werden");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
        return false;
    }

    beginRemoveRows(parent, row, row + count - 1);
    if(!mBookTable->remove(mBooks.at(row).isbn))
    {
        // if it fails just return
        endRemoveRows();
        return false;
    }
    // else fix the model
    mBooks.removeAt(row);
    endRemoveRows();
    return true;
}

//---------------------------
// reset(): clears the model so that new data can be set
//---------------------------
void BookModel::reset()
{
    beginResetModel();
    mBooks.clear();
    endResetModel();
}

//---------------------------
// search(): searches for the given string
//---------------------------
void BookModel::search(QString searchstring)
{
    beginResetModel();
    mBooks.clear();
    QList<BookTable::BookRow> srows;

    // When the searchstring is empty just return all students again
    if(searchstring.isEmpty())
    {
        srows = mBookTable->getAllBooks();
    }
    else
    {
        srows = mBookTable->searchBookByTitle(searchstring);
        QList<BookTable::BookRow> temprows;
        temprows = mBookTable->searchBooksBySubject(searchstring);
        // Check for duplicates
        for(BookTable::BookRow newRow : temprows)
        {
            if(!srows.contains(newRow))
            {
                srows.append(newRow);
            }
        }
    }

    // copy the elements
    for(BookTable::BookRow srow : srows)
    {
        mBooks.append(srow);
    }
    endResetModel();
}

//---------------------------
// getRow returns the BookTable::BookRow for the given index
//---------------------------
BookTable::BookRow* BookModel::getRow(QModelIndex row)
{
    if (!row.isValid())
        return nullptr;

    return &(mBooks[row.row()]);
}

//--------------------------
// updateRow updates the row
//--------------------------
void BookModel::updateRow(int row)
{
    emit dataChanged(this->index(row,0), this->index(row, 5));
}

//--------------------------
// addRow adds a new row to the model
//--------------------------
void BookModel::addRow(BookTable::BookRow * newRow)
{
    emit beginInsertRows(QModelIndex(), rowCount(), rowCount());
    mBooks.append(*newRow);
    emit endInsertRows();
}
