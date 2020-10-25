#include "lendmodel.h"
#include <QRegExp>
#include <QIcon>
#include <QPainter>
#include <QApplication>
#include <QDateTime>
#include "damagedialog.h"
#include <QMessageBox>


//============================
// Implementations of LendModel
//============================
//----------------------------
// Constructor
//----------------------------
LendModel::LendModel(StudentTable::StudentRow* srow, StudentTable* stable, BookTable* btable, LendingTable* ltable,
                           QFontMetrics font, QObject * parent) :
    QAbstractTableModel(parent), mBookTable(btable), mLendingTable(ltable), mStudentTable(stable),
    mGrade(-1), mFont(font), mParent(nullptr)
{
    StudentWithLendings.prename = srow->preName;
    StudentWithLendings.surname = srow->surName;
    StudentWithLendings.dateofbirth = srow->dateOfBirth;
    StudentWithLendings.address = srow->address;
    StudentWithLendings.classRows = srow->classRows;
    StudentWithLendings.comment = srow->comment;
    QList<LendingTable::LendingRow> lrows = mLendingTable->getOpenLendingsFromStudent(srow->preName, srow->surName, srow->dateOfBirth);
    LendingTable::LendingRow lrow;
    foreach(lrow, lrows)
    {
        if (StudentWithLendings.lendings.contains(lrow.BookIsbn))
            StudentWithLendings.lendings[lrow.BookIsbn].id.append(lrow.ID);
        else
        {
            LendInfo info; info.id.append( lrow.ID);
            StudentWithLendings.lendings.insert(lrow.BookIsbn, info);
            Book bk;
            bk.isbn = lrow.BookIsbn;
            bk.title = mBookTable->getBook(lrow.BookIsbn)->title;
            mBooks.append(bk);
        }
    }
    QList<ClassTable::ClassRow> classes = mStudentTable->getClassesFromStudent(srow->preName, srow->surName, srow->dateOfBirth);
    ClassTable::ClassRow cl;
    foreach(cl, classes)
    {
        QList<BookTable::BookRow> books;
        if(!cl.subject.isEmpty())
            books = mBookTable->searchBooksBySubject(cl.subject);
        else
            books = mBookTable->getBooksByGrade(cl.grade);
        BookTable::BookRow book;
        foreach(book, books)
        {
            Book bk;
            bk.isbn = book.isbn;
            bk.title = book.title;
            Book bk2;
            bool inList = false;
            foreach(bk2, mBooks)
            {
                if(bk2.isbn == bk.isbn)
                    inList = true;
            }
            if(!inList)
                mBooks.append(bk);
        }
    }
}

//--------------------------
// Destructor
//--------------------------
LendModel::~LendModel()
{
    mStudentTable = nullptr;
    mBookTable = nullptr;
    mLendingTable = nullptr;
}

//--------------------------
// rowCount(): returns the number of rows
//--------------------------
int LendModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mBooks.count();
    }

//---------------------------
// columnCount(): returns the number of columns
//---------------------------
int LendModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

//---------------------------
// data(): which returns all informations to display
//---------------------------
QVariant LendModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() > mBooks.count())
        return QVariant();

    if (index.column() > 2)
        return QVariant();

    switch(role)
    {
        case Qt::DisplayRole:
        {
            if (index.column() != 0)
                return QVariant();
            Book bk = mBooks.at(index.row());
            return bk.title;
        }
        case Qt::CheckStateRole:
        {
            if (index.column() != 1)
                return QVariant();
            if (StudentWithLendings.lendings.contains(mBooks.at(index.row()).isbn))
                return StudentWithLendings.lendings[mBooks.at(index.row()).isbn].id.size();
            else
                return 0;
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
// lendNewBook(): lends the book for the student
//---------------------------
bool LendModel::lendNewBook(Book book, Student &student)
{
    int id;
    if(!mLendingTable->insertLending(&id, 0, StudentWithLendings.prename, student.surname,
                                          student.dateofbirth,
                                          book.isbn, LendingTable::NORMAL,
                                          QDate::currentDate().toString("dd.MM.yyyy"),
                                 "31.07." + QString::number(QDate::currentDate().year())))
        return false;

    if (student.lendings.contains(book.isbn))
    {
        student.lendings[book.isbn].id.append(id);
        if (!student.comment.contains("Mehrfach ausgeliehene(s) Buch/Bücher"))
        {
            if (!student.comment.isEmpty())
                student.comment += "; ";
            student.comment += "Mehrfach ausgeliehene(s) Buch/Bücher";
            mStudentTable->updateStudentComment(student.prename, student.surname, student.dateofbirth,student.comment);
        }
    }
    else
    {
        LendInfo info;
        info.id.append(id);
        student.lendings.insert(book.isbn, info);
    }

    // update the available amount of the book
    BookTable::BookRow *row = mBookTable->getBook(book.isbn);
    if (row)
    {
        int amount = row->available;
        mBookTable->updateAvailable(book.isbn, amount - 1);
    }

    return true;
}

//---------------------------
// giveBackBook(): gives the book back for the student
//---------------------------
bool LendModel::giveBackBook(Book book, Student &student, QString damage)
{
    bool rueckgabe = false;
    LendingTable::LendingRow * togivebackrow = mLendingTable->getLending(student.lendings[book.isbn].id.at(0));
    QDate lastReturnDate = QDate::fromString(togivebackrow->LendDate, "dd.MM.yyyy");

    for (int i = 1; i < student.lendings[book.isbn].id.size(); i++)
    {
        LendingTable::LendingRow * row = mLendingTable->getLending(student.lendings[book.isbn].id.at(i));
        QDate next = QDate::fromString(row->LendDate, "dd.MM.yyyy");
        if (next < lastReturnDate)
        {
            delete togivebackrow;
            lastReturnDate = next;
            togivebackrow = row;
        }
        else
            delete row;
    }
    GiveBackTable::GiveBackRow add;
    add.Damage = damage;
    add.Date = QDate::currentDate().toString("dd.MM.yyyy");
    togivebackrow->GiveBackRow = add;
    if (mLendingTable->insertGiveBack(togivebackrow))
    {
        student.lendings[book.isbn].id.removeAll(togivebackrow->ID);
        if (student.lendings[book.isbn].id.size() == 0)
        {
            student.lendings.remove(book.isbn);
        }
        if (!student.lendings.contains(book.isbn) || student.lendings[book.isbn].id.size() < 2)
        {
            bool hasFurtherDoubleLendings = false;
            foreach(const LendInfo value, student.lendings)
            {
                if (value.id.size() > 1)
                {
                    hasFurtherDoubleLendings = true;
                    break;
                }
            }
            if (!hasFurtherDoubleLendings)
            {
                student.comment = student.comment.replace("; Mehrfach ausgeliehene(s) Buch/Bücher", "");
                student.comment = student.comment.replace("Mehrfach ausgeliehene(s) Buch/Bücher", "");
                mStudentTable->updateStudentComment(student.prename ,student.surname, student.dateofbirth,student.comment);
            }
        }
        rueckgabe = true;
    }
    delete togivebackrow;

    // update the available number of the book
    if (rueckgabe)
    {
        BookTable::BookRow *row = mBookTable->getBook(book.isbn);
        if (row)
        {
            int amount = row->available;
            mBookTable->updateAvailable(book.isbn, amount + 1);
        }
    }

    return rueckgabe;
}

//---------------------------
// setData sets the data on the model
// for role = Qt::CheckStateRole and value == 1 add a booking and for value == -1 remove last booking
//---------------------------
bool LendModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (role != Qt::CheckStateRole)
        return false;

    if (index.column() < 1)
        return false;

    if (value.toInt() == 1)
    {
        // add a new booking
        Book book = mBooks.at(index.row());
        if (!lendNewBook(book, StudentWithLendings))
            return false;
        QVector<int> vect;
        vect.append(Qt::CheckStateRole);
        emit dataChanged(index, index, vect);
        return true;
    }

    if (value.toInt() == -1)
    {
        // return a book
        Book book = mBooks.at(index.row());
        if (!StudentWithLendings.lendings.contains(book.isbn))
            return false;

        DamageDialog dialog(book.title, mParent);
        if (dialog.exec() != QDialog::Accepted)
            return false;

        if(giveBackBook(book, StudentWithLendings, dialog.getDamage()))
        {
            QVector<int> vect;
            vect.append(Qt::CheckStateRole);
            emit dataChanged(index, index, vect);
            return true;
        }
    }

    return false;
}

//----------------------------
// flags returns the flag for each cell
//----------------------------
Qt::ItemFlags LendModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    if (index.column() < 1)
        return QAbstractTableModel::flags(index);
    else
        return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable;
}

//=============================
// Implementations of ItemDelegate
//=============================
//-----------------------------
// Constructor
//-----------------------------
ItemDelegate::ItemDelegate(QObject *parent)
        : QStyledItemDelegate(parent)
{
}

//------------------------------
// paint draws the items centered
//------------------------------
void ItemDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QStyleOptionViewItemV4 viewItemOption(option);
    if (index.column() == 0)
        QStyledItemDelegate::paint(painter, viewItemOption, index);

    if (index.column() == 1)
    {
        // get the full rect and split it up to three
        QRect fullRect = option.rect;
        int width = fullRect.width() / 3;
        QRect rectMinus = QRect(fullRect.x(), fullRect.y(), width, fullRect.height());
        QRect rectText = QRect(fullRect.x()+ width, fullRect.y(), width + fullRect.width()%3, fullRect.height());
        QRect rectPlus = QRect(rectText.width() + rectText.x(), fullRect.y(), width, fullRect.height());

        QStyleOptionButton button;
        button.state = QStyle::State_Enabled;
        button.direction = option.direction;


        button.text = "-";
        button.rect = rectMinus;
        QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);


        int amount = index.data(Qt::CheckStateRole).toInt();
        QRect alignedRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                         QSize(rectText.width()/2,rectText.height()), rectText);

        painter->drawText(alignedRect,Qt::AlignLeft, QString::number(amount));

        button.text = "+";
        button.rect = rectPlus;
        QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
    }
}

//------------------------------
// ediortEvent called when the user clicks on a cell
//------------------------------
bool ItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    // make sure that the item is checkable
    Qt::ItemFlags flags = model->flags(index);
    if (!(flags & Qt::ItemIsUserCheckable) || !(flags & Qt::ItemIsEnabled))
        return false;

    // make sure that we have a check state
    QVariant value = index.data(Qt::CheckStateRole);
    if (!value.isValid())
        return false;

    // make sure that we have the right event type
    if (event->type() == QEvent::MouseButtonRelease)
    {
        {
            QRect fullRect = option.rect;
            int width = fullRect.width() / 3;
            QRect rectMinus = QRect(fullRect.x(), fullRect.y(), width, fullRect.height());
            QRect rectText = QRect(fullRect.x()+ width, fullRect.y(), width + fullRect.width()%3, fullRect.height());
            QRect rectPlus = QRect(rectText.width() + rectText.x(), fullRect.y(), width, fullRect.height());

            if (rectMinus.contains(static_cast<QMouseEvent*>(event)->pos()))
                return model->setData(index, QVariant(-1), Qt::CheckStateRole);

            if (rectPlus.contains(static_cast<QMouseEvent*>(event)->pos()))
                return model->setData(index, 1, Qt::CheckStateRole);
        }
    }
    else if (event->type() == QEvent::KeyPress)
    {
        if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space&& static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
            return false;
    }
    else
    {
            return false;
    }
    return false;
}
