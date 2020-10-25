#include "booklendmodel.h"
#include <QDateTime>
#include <QPainter>
#include <QApplication>


//============================
// Implementations of BookLendModel
//============================
//----------------------------
// Constructor
//----------------------------
BookLendModel::BookLendModel(QString classname, QList<StudentTable::StudentRow> srows, BookTable* btable, LendingTable* ltable,
                             StudentTable * stable, ClassTable* ctable, QFontMetrics font, QObject * parent) :
    QAbstractTableModel(parent), classname(classname), msrows(srows), mBookTable(btable), mLendingTable(ltable),
    mStudentTable(stable), mClassTable(ctable), mGrade(-1), mFont(font), mParent(nullptr)
{
    StudentTable::StudentRow srow;
    Student student;
    foreach(srow, msrows)
    {
        student.address = srow.address;
        student.classRows = srow.classRows;
        student.comment = srow.comment;
        student.dateofbirth = srow.dateOfBirth;
        student.prename = srow.preName;
        student.surname = srow.surName;
        student.lendings.clear();
        QList<LendingTable::LendingRow> lrows = mLendingTable->getOpenLendingsFromStudent(srow.preName, srow.surName, srow.dateOfBirth);
        LendingTable::LendingRow lrow;
        foreach(lrow, lrows)
        {
            if (student.lendings.contains(lrow.BookIsbn))
                student.lendings[lrow.BookIsbn].id.append(lrow.ID);
            else
            {
                LendInfo info; info.id.append( lrow.ID);
                student.lendings.insert(lrow.BookIsbn, info);
            }
        }
        StudentsWithLendings.append(student);
    }

    QString subject = mClassTable->getClass(classname)->subject;
    QList<BookTable::BookRow> bookBuffer;
    if(subject.isEmpty())
        bookBuffer = mBookTable->getBooksByGrade(mClassTable->getClass(classname)->grade);
    else
        bookBuffer = mBookTable->searchBooksBySubject(subject);
    BookTable::BookRow oneBookRow;
    Book oneBook;
    foreach(oneBookRow, bookBuffer)
    {
        oneBook.isbn = oneBookRow.isbn;
        oneBook.title = oneBookRow.title;
        mBooks.append(oneBook);
    }
}

//--------------------------
// Destructor
//--------------------------
BookLendModel::~BookLendModel()
{
    mBookTable = nullptr;
    mLendingTable = nullptr;
    mStudentTable = nullptr;
    mClassTable = nullptr;
}

//--------------------------
// rowCount(): returns the number of rows
//--------------------------
int BookLendModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mBooks.count();
    }

//---------------------------
// columnCount(): returns the number of columns
//---------------------------
int BookLendModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

//---------------------------
// data(): which returns all informations to display
//---------------------------
QVariant BookLendModel::data(const QModelIndex &index, int role) const
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
            int lendingsCounter = -1;
            QString bookisbn = mBooks.at(index.row()).isbn;
            for(int a = 0; a<StudentsWithLendings.length(); a += 1)
            {
                if(!StudentsWithLendings[a].lendings.contains(bookisbn))
                {
                    lendingsCounter = 0;
                }
                int counter = StudentsWithLendings[a].lendings[bookisbn].id.count();
                if(lendingsCounter == -1 || lendingsCounter > counter)
                    lendingsCounter = counter;
            }
            return lendingsCounter;
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
// lendNewBook(): lends the new book for all students
//---------------------------
bool BookLendModel::lendNewBooks(Book book)
{
    int counter;
    for(int a = 0; a<StudentsWithLendings.length();a+=1)
    {
        int id;
        if(!mLendingTable->insertLending(&id, 0, StudentsWithLendings[a].prename, StudentsWithLendings[a].surname,
                                              StudentsWithLendings[a].dateofbirth,
                                              book.isbn, LendingTable::NORMAL,
                                              QDate::currentDate().toString("dd.MM.yyyy"),
                                     "31.07." + QString::number(QDate::currentDate().year())))
            return false;

        if (StudentsWithLendings[a].lendings.contains(book.isbn))
        {
            StudentsWithLendings[a].lendings[book.isbn].id.append(id);
            if (!StudentsWithLendings[a].comment.contains("Mehrfach ausgeliehene(s) Buch/Bücher"))
            {
                if (!StudentsWithLendings[a].comment.isEmpty())
                    StudentsWithLendings[a].comment += "; ";
                StudentsWithLendings[a].comment += "Mehrfach ausgeliehene(s) Buch/Bücher";
                mStudentTable->updateStudentComment(StudentsWithLendings[a].prename, StudentsWithLendings[a].surname,
                                                    StudentsWithLendings[a].dateofbirth,StudentsWithLendings[a].comment);
            }
        }
        else
        {
            LendInfo info;
            info.id.append(id);
            StudentsWithLendings[a].lendings.insert(book.isbn, info);
        }
        counter += 1;
    }
    // update the available amount of the book
    BookTable::BookRow *row = mBookTable->getBook(book.isbn);
    if (row)
    {
        int amount = row->available;
        mBookTable->updateAvailable(book.isbn, amount - counter);
    }

    return true;
}

//---------------------------
// giveBackBook(): gives the book back for all students
//---------------------------
bool BookLendModel::giveBackBooks(Book book)
{
    int counter = 0;
    bool rueckgabe = false;
    for(int a = 0; a<StudentsWithLendings.length();a+=1)
    {
        rueckgabe = false;
        LendingTable::LendingRow * togivebackrow = mLendingTable->getLending(StudentsWithLendings[a].lendings[book.isbn].id.at(0));
        QDate lastReturnDate = QDate::fromString(togivebackrow->LendDate, "dd.MM.yyyy");

        for (int i = 1; i < StudentsWithLendings[a].lendings[book.isbn].id.size(); i++)
        {
            LendingTable::LendingRow * row = mLendingTable->getLending(StudentsWithLendings[a].lendings[book.isbn].id.at(i));
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
        add.Damage = "";
        add.Date = QDate::currentDate().toString("dd.MM.yyyy");
        togivebackrow->GiveBackRow = add;
        if (mLendingTable->insertGiveBack(togivebackrow))
        {
            StudentsWithLendings[a].lendings[book.isbn].id.removeAll(togivebackrow->ID);
            if (StudentsWithLendings[a].lendings[book.isbn].id.size() == 0)
            {
                StudentsWithLendings[a].lendings.remove(book.isbn);
            }
            if (!StudentsWithLendings[a].lendings.contains(book.isbn) || StudentsWithLendings[a].lendings[book.isbn].id.size() < 2)
            {
                bool hasFurtherDoubleLendings = false;
                foreach(const LendInfo value, StudentsWithLendings[a].lendings)
                {
                    if (value.id.size() > 1)
                    {
                        hasFurtherDoubleLendings = true;
                        break;
                    }
                }
                if (!hasFurtherDoubleLendings)
                {
                    StudentsWithLendings[a].comment = StudentsWithLendings[a].comment.replace("; Mehrfach ausgeliehene(s) Buch/Bücher", "");
                    StudentsWithLendings[a].comment = StudentsWithLendings[a].comment.replace("Mehrfach ausgeliehene(s) Buch/Bücher", "");
                    mStudentTable->updateStudentComment(StudentsWithLendings[a].prename ,StudentsWithLendings[a].surname,
                                                        StudentsWithLendings[a].dateofbirth,StudentsWithLendings[a].comment);
                }
            }
            rueckgabe = true;
        }
        delete togivebackrow;
        counter += 1;
    }
    // update the available number of the book
    if (rueckgabe)
    {
        BookTable::BookRow *row = mBookTable->getBook(book.isbn);
        if (row)
        {
            int amount = row->available;
            mBookTable->updateAvailable(book.isbn, amount + counter);
        }
    }

    return rueckgabe;
}

//---------------------------
// setData(): sets the data on the model that means
// for role = Qt::CheckStateRole and value == 1 add a booking and for value == -1 remove last booking
//---------------------------
bool BookLendModel::setData(const QModelIndex &index, const QVariant &value, int role)
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
        if (!lendNewBooks(book))
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
        for(int a = 0; a<StudentsWithLendings.length(); a += 1)
        {
            if(!StudentsWithLendings[a].lendings.contains(book.isbn))
                return false;
        }

        if(giveBackBooks(book))
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
// flags(): returns the flag for each cell
//----------------------------
Qt::ItemFlags BookLendModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    if (index.column() < 1)
        return QAbstractTableModel::flags(index);
    else
        return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable;
}

//=============================
// Implementations of BookItemDelegate
//=============================
//-----------------------------
// Konstruktor
//-----------------------------
BookItemDelegate::BookItemDelegate(QObject *parent)
        : QStyledItemDelegate(parent)
{
}

//------------------------------
// paint(): draws the items centered
//------------------------------
void BookItemDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
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
// ediortEvent(): called when the user clicks on a cell
//------------------------------
bool BookItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
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

