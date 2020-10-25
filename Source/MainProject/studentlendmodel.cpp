#include "studentlendmodel.h"
#include <QRegExp>
#include <QIcon>
#include <QPainter>
#include <QApplication>
#include <QDateTime>
#include "damagedialog.h"
#include <QMessageBox>
#include <QToolTip>

//============================
// helper functions
//============================


//============================
// Implementations of StudentModel
//============================
//----------------------------
// Konstructor
//----------------------------
StudentLendModel::StudentLendModel(StudentTable* stable, BookTable* btable, LendingTable* ltable,
                           QFontMetrics font, QObject * parent) :
    QAbstractTableModel(parent), mStudentTable(stable), mBookTable(btable), mLendingTable(ltable),
    mGrade(-1), mFont(font), mParent(nullptr)
{
}

//--------------------------
// Destructor
//--------------------------
StudentLendModel::~StudentLendModel()
{
    mStudentTable = nullptr;
    mBookTable = nullptr;
    mLendingTable = nullptr;
}

//--------------------------
// rowCount
// returns the number of rows
//--------------------------
int StudentLendModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mStudents.count();
}

//---------------------------
// columnCount
// returns the number of columns meaning the number of books
//---------------------------
int StudentLendModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    int count = mBooks.count();
    return count;
}

//---------------------------
// data which returns all informations to display
//---------------------------
QVariant StudentLendModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= mStudents.count())
        return QVariant();

    if (index.column() >= mBooks.count())
        return QVariant();

    switch(role)
    {
        case Qt::DisplayRole:
        {
            return QVariant();
        }
        case Qt::CheckStateRole:
        {
            if (mStudents.at(index.row()).lendings.contains(mBooks.at(index.column()).isbn))
                return mStudents.at(index.row()).lendings[mBooks.at(index.column()).isbn].id.size();
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
// headerData returns all information to display in the header
//---------------------------
QVariant StudentLendModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::CheckStateRole && role != Qt::DecorationRole && role != Qt::ToolTipRole)
        return QVariant();

    if (orientation == Qt::Vertical)
    {
        if (role == Qt::DisplayRole)
        {
            Student st = mStudents.at((section));
            return st.surname + ", " + st.forename;
        }

        else if (role == Qt::ToolTipRole)
        {
            if(!mStudents.at(section).comment.isEmpty())
                return mStudents.at(section).comment;
            else
                return QVariant();
        }


        else if(role == Qt::DecorationRole)
        {
            if(!mStudents.at(section).comment.isEmpty())
                return QIcon(":/icons/icons/Statusinformation.png");
            else
                return QVariant();
        }

        else
            return QVariant();
    }

    if (role == Qt::CheckStateRole)
    {
        return areAllItemsOfSectionChecked(section);
    }

    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        return mBooks.at(section).title;
    }

    return QVariant();
}

//---------------------------
// lendNewBook
// Attention: reference parameter!
//---------------------------
bool StudentLendModel::lendNewBook(Book book, Student &student)
{
    int id;
    if(!mLendingTable->insertLending(&id, 0, student.forename,student.surname,
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
            mStudentTable->updateStudentComment(student.forename,student.surname, student.dateofbirth,student.comment);
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
// giveBackBook
// Attention: student is a reference parameter
//---------------------------
bool StudentLendModel::giveBackBook(Book book, Student &student, QString damage)
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
                mStudentTable->updateStudentComment(student.forename,student.surname, student.dateofbirth,student.comment);
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
// setData sets the data on the model that means
// for role = Qt::CheckStateRole and value == 1 add a booking and for value == -1 remove last booking
//---------------------------
bool StudentLendModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    if (role != Qt::CheckStateRole)
        return false;

    if (value.toInt() == 1)
    {
        // add a new booking
        Student &student = mStudents[index.row()];
        Book book = mBooks.at(index.column());
        if (!lendNewBook(book, student))
            return false;
        QVector<int> vect;
        vect.append(Qt::CheckStateRole);
        emit dataChanged(index, index, vect);
        emit headerDataChanged(Qt::Horizontal, index.column(), index.column());
        return true;
    }

    if (value.toInt() == -1)
    {
        // return a book
        Student &student = mStudents[index.row()];
        Book book = mBooks.at(index.column());
        if (!student.lendings.contains(book.isbn))
            return false;

        DamageDialog dialog(student.forename, student.surname, book.title, mParent);
        if (dialog.exec() != QDialog::Accepted)
            return false;

        if(giveBackBook(book,student, dialog.getDamage()))
        {
            QVector<int> vect;
            vect.append(Qt::CheckStateRole);
            emit dataChanged(index, index, vect);
            emit headerDataChanged(Qt::Horizontal, index.column(), index.column());
            return true;
        }
    }

    return false;
}

//----------------------------
// flags returns the flag for each cell
//----------------------------
Qt::ItemFlags StudentLendModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable;
}

//-----------------------------
// setHeaderData
//-----------------------------
bool StudentLendModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (section > mBooks.size())
        return false;

    if (role != Qt::CheckStateRole)
        return false;

    if (orientation != Qt::Horizontal)
        return false;

    Book book = mBooks.at(section);
    QVector<int> vect;
    vect.append(Qt::CheckStateRole);

    switch(value.toInt())
    {
    case Qt::Checked:
    {
        for (int i = 0; i < mStudents.size(); i++)
        {
            Student &student = mStudents[i]; // this does not work!
            lendNewBook(book, student);
            emit dataChanged(index(i, section),index(i, section), vect);
        }
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    case Qt::Unchecked:
    {
        QMessageBox box(QMessageBox::Question, "Rückgabe durchführen?", "Wollen Sie wirklich das Buch von allen Schülern zurückgeben?", QMessageBox::NoButton, mParent );
        QAbstractButton* pButtonYes = (QAbstractButton*)box.addButton("Ja", QMessageBox::AcceptRole);
        box.addButton("Nein", QMessageBox::RejectRole);
        box.exec();
        if (box.clickedButton() != pButtonYes)
            return false;

        for (int i = 0; i < mStudents.size(); i++)
        {
            Student &student = mStudents[i];
            if (!student.lendings.contains(book.isbn))
                continue;
            if (giveBackBook(book, student, ""))
            {
                emit dataChanged(index(i, section),index(i, section), vect);
            }
        }
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    }

    return false;
}

//---------------------------
// selectClass queries information about a specified class
//---------------------------
void StudentLendModel::selectClass(QString classname, int grade, QString subject, BookOption mOption)
{
        beginResetModel();
        mClass = classname;
        mGrade = grade;
        mSubject = subject;
        mStudents.clear();
        mBooks.clear();

        QList<StudentTable::StudentRow> srows = mStudentTable->getStudentsByClassName(mClass);
        StudentTable::StudentRow srow;
        foreach(srow, srows)
        {
            Student st;
            st.comment = srow.comment;
            st.dateofbirth = srow.dateOfBirth;
            st.forename = srow.preName;
            st.surname = srow.surName;
            QList<LendingTable::LendingRow> lrows = mLendingTable->getOpenLendingsFromStudent(st.forename, st.surname , st.dateofbirth);
            LendingTable::LendingRow lrow;
            foreach(lrow, lrows)
            {
                if (st.lendings.contains(lrow.BookIsbn))
                    st.lendings[lrow.BookIsbn].id.append(lrow.ID);
                else
                {
                    LendInfo info; info.id.append( lrow.ID);
                    st.lendings.insert(lrow.BookIsbn, info);
                }
            }
            mStudents.append(st);
        }

        QList<BookTable::BookRow> rows;
        if (mOption == BooksForClass)
            rows = mBookTable->getBooksByGrade(mGrade);
        else if (mOption == LentBooks)
        {
            QList<LendingTable::LendingRow> openClassLendings;
            foreach(Student student, mStudents)
                openClassLendings = openClassLendings + mLendingTable->getOpenLendingsFromStudent(student.forename, student.surname, student.dateofbirth);
            QSet<QString> isbns;
            foreach(LendingTable::LendingRow lending, openClassLendings)
                isbns.insert(lending.BookIsbn);
            foreach(QString isbn, isbns)
                rows.append(*mBookTable->getBook(isbn));
        }
        else if (mOption == AllBooks)
            rows = mBookTable->getAllBooks();
        BookTable::BookRow row;
        foreach(row, rows)
        {       
            Book book;
            book.isbn = row.isbn;
            book.title = row.title;
            if (mOption == BooksForClass && !subject.isEmpty())
            {
                if (row.subject == subject)
                    mBooks.append(book);
            }
            else
                mBooks.append(book);
        }

        endResetModel();
        emit columnsmaychanged();
}

//---------------------------
// reset clears the model so that new data can be set
//---------------------------
void StudentLendModel::reset()
{
    beginResetModel();
    mStudents.clear();
    endResetModel();
    emit columnsmaychanged();
}

//----------------------------
// setDialogParent
//----------------------------
void StudentLendModel::setDialogParent(QWidget *parent)
{
    mParent = parent;
}

//----------------------------
// areAllItemsOfSectionChecked
//----------------------------
bool StudentLendModel::areAllItemsOfSectionChecked(int section) const
{
    bool allSelected = true;
    Student row;
    QString isbn = mBooks.at(section).isbn;
    foreach(row, mStudents)
    {
        if (!row.lendings.contains(isbn))
        {
            allSelected = false;
            break;
        }
    }
    return allSelected;
}

//----------------------------
// searchInBooks:
//----------------------------
void StudentLendModel::searchInBooks(QString searchString, BookOption mOption)
{
    beginResetModel();
    mBooks.clear();

    QList<BookTable::BookRow> rows;

    //check searchstring
    if(searchString.isEmpty())
    {
        if (mOption == BooksForClass)
            rows = mBookTable->getBooksByGrade(mGrade);
        else if (mOption == LentBooks)
        {
            QList<LendingTable::LendingRow> openClassLendings;
            foreach(Student student, mStudents)
                openClassLendings = openClassLendings + mLendingTable->getOpenLendingsFromStudent(student.forename, student.surname, student.dateofbirth);
            QSet<QString> isbns;
            foreach(LendingTable::LendingRow lending, openClassLendings)
                isbns.insert(lending.BookIsbn);
            foreach(QString isbn, isbns)
                rows.append(*mBookTable->getBook(isbn));
        }
        else if (mOption == AllBooks)
            rows = mBookTable->getAllBooks();
        BookTable::BookRow row;
        foreach(row, rows)
        {
            Book book;
            book.isbn = row.isbn;
            book.title = row.title;
            mBooks.append(book);

        }
        endResetModel();
        emit columnsmaychanged();
        return;
    }
    QList<BookTable::BookRow> books = mBookTable->searchBookByTitle(searchString);
    for(int i = 0; i<books.length(); i++)
        rows.append(books[i]);
    books = mBookTable->searchBooksBySubject(searchString);
    for(int i = 0; i<books.length(); i++)
    {
        bool existing = false;
        foreach(BookTable::BookRow row, rows)
        {
            if(books[i].isbn == row.isbn)
                existing = true;
        }
        if(existing == false)
            rows.append(books[i]);
    }
    if (mOption == BooksForClass)
    {
        for(int i = 0; i<rows.size(); i++)
        {
            if(rows.at(i).grade.contains(mGrade))
                continue;
            else
            {
                rows.removeAt(i);
                i--;
            }
        }
    }
    else if (mOption == LentBooks)
    {
        //TODO
    }
    BookTable::BookRow row;
    foreach(row, rows)
    {
        Book book;
        book.isbn = row.isbn;
        book.title = row.title;
        mBooks.append(book);
    }

    endResetModel();
    emit columnsmaychanged();
}

//----------------------------
// getStudent returns a StudentRow at the specified index
//----------------------------
StudentTable::StudentRow* StudentLendModel::getStudent(QModelIndex index)
{
    StudentTable::StudentRow* row = nullptr;
    Student student = mStudents.at(index.row());
    row = mStudentTable->getStudent(student.forename, student.surname, student.dateofbirth);
    return row;
}

//=============================
// Implementations of ItemDelegate
//=============================
//-----------------------------
// Konstruktor
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

    /*if (index.column() == 0)
    {
        QRect newRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
               QSize(option.decorationSize.width() + 5,option.decorationSize.height()),
               QRect(option.rect.x(), option.rect.y(),
               option.rect.width(), option.rect.height()));
        viewItemOption.rect = newRect;
        QStyledItemDelegate::paint(painter, viewItemOption, index);
    }

    if (index.column() == 1)
        QStyledItemDelegate::paint(painter, viewItemOption, index);*/

    //if (index.column() > 1)
    {
        const int textMargin = 3;
        QStyleOptionButton button;
        button.state = QStyle::State_Enabled;
        QSize size (20,20);
        button.iconSize = size;
        button.direction = option.direction;
        QRect newRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                                                QSize(62,20),
                                                QRect(option.rect.x() + textMargin, option.rect.y(),
                                                      option.rect.width() - (2 * textMargin), option.rect.height()));

        button.text = "-";
        button.rect = QRect(newRect.x(),newRect.y(), size.width(), size.height());
        QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
        int amount = index.data(Qt::CheckStateRole).toInt();
        if (amount > 1)
            painter->drawText(QRect(newRect.x()+size.width()+1, newRect.y(), size.width(), size.height()),Qt::AlignLeft, QString::number(amount));
        else
        {
            QStyleOptionButton checkbox;
            checkbox.iconSize = size;
            checkbox.rect = QRect(newRect.x()+size.width()+1, newRect.y(), size.width(), size.height());
            checkbox.state = QStyle::State_Enabled | QStyle::State_Active;

            if (amount == 1)
                checkbox.state |= QStyle::State_On;
            else
                checkbox.state |= QStyle::State_Off;

            QApplication::style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &checkbox, painter);
        }
        button.text = "+";
        button.rect = QRect(newRect.x()+ 2*(size.width()+1),newRect.y(), size.width(), size.height());
        QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
    }
}

//------------------------------
// ediortEvent called when the user clicks on a cell
// if the user clicks on the checkbox setdata should be called
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
        /*if (index.column() < 2)
        {
            const int textMargin = 3;
            QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                                option.decorationSize,
                                QRect(option.rect.x() + (2 * textMargin), option.rect.y(),
                                option.rect.width() - (2 * textMargin),
                                option.rect.height()));
            if (!checkRect.contains(static_cast<QMouseEvent*>(event)->pos()))
                return false;
        }
        else*/
        {
            const int textMargin = 3;
            QSize size (20,20);
            QRect newRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                             QSize(62,20),QRect(option.rect.x() + textMargin, option.rect.y(),
                             option.rect.width() - (2 * textMargin), option.rect.height()));
            QRect buttonMinus (newRect.x(),newRect.y(), size.width(), size.height());
            if (buttonMinus.contains(static_cast<QMouseEvent*>(event)->pos()))
                return model->setData(index, QVariant(-1), Qt::CheckStateRole);

            int amount = index.data(Qt::CheckStateRole).toInt();
            if (amount > 1)
                return false;
            else
            {
                QRect checkbox(newRect.x()+size.width()+1, newRect.y(), size.width(), size.height());
                if (checkbox.contains(static_cast<QMouseEvent*>(event)->pos()))
                {
                    int value = 0;
                    if (amount == 0)
                        value = 1;
                    else
                        value = -1;
                    return model->setData(index, value, Qt::CheckStateRole);
                }
            }
            QRect buttonPlus(newRect.x()+ 2*(size.width()+1),newRect.y(), size.width(), size.height());
            if (buttonPlus.contains(static_cast<QMouseEvent*>(event)->pos()))
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


//===============================
// Implementations of CheckBoxHeader
//===============================
//-------------------------------
// Konstruktor
//-------------------------------
CheckBoxHeader::CheckBoxHeader(Qt::Orientation orientation, QWidget* parent /*= 0*/)
    : QHeaderView(orientation, parent)
{
    setMinimumSectionSize(70);
}

//-------------------------------
// paintSection draws the item
//-------------------------------
void CheckBoxHeader::paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const
{
    // TODO: drawtext with wordrap or rotate text according to f(w, h, alpha) = w cos (alpha) + h cos (90 - alpha)
    //if (logicalIndex > 1)
    {
        QStyleOptionButton option;
        QRect rectText(rect.x(), rect.y(), rect.width(), rect.height()/2);
        QRect rectCheckbox(rect.x(), rect.y()+ rect.height()/2, rect.width(), rect.height()/2);
        option.rect = rectCheckbox;
        option.state = QStyle::State_Enabled | QStyle::State_Active;

        if (model()->headerData(logicalIndex, this->orientation(), Qt::CheckStateRole).toBool())
            option.state |= QStyle::State_On;
        else
            option.state |= QStyle::State_Off;

        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter);
        painter->drawText(rectText,Qt::AlignCenter, model()->headerData(logicalIndex, Qt::Horizontal,Qt::DisplayRole ).toString());
    }
    /*else
        QHeaderView::paintSection(painter, rect, logicalIndex);*/
}

//-------------------------------
// mousePressEvent
//-------------------------------
void CheckBoxHeader::mousePressEvent(QMouseEvent* event)
{
    int section = logicalIndexAt( event->pos() );

    int height = this->height() - contentsMargins().bottom()- contentsMargins().top();
    int width = sectionSize(section);
    int x = -offset();
    for (int i = 0; i < section; i++)
        x += (sectionSize(i) - contentsMargins().left() - contentsMargins().right());

    // make sure that we have a check state
    QVariant value = model()->headerData(section, orientation(), Qt::CheckStateRole);
    if (!value.isValid())
        return;

    QRect checkboxRect(x, height/2, width, height/2);
    QRect checkRect = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                QSize(25,25),
                                checkboxRect);

    if (!checkRect.contains(event->pos()))
        return;

    Qt::CheckState state = (static_cast<Qt::CheckState>(value.toBool())
                       ? Qt::Unchecked : Qt::Checked);
    model()->setHeaderData(section, orientation(), state, Qt::CheckStateRole);
}

//-------------------------------
// sizeHint
//-------------------------------
QSize CheckBoxHeader::sizeHint() const
{
    // TODO!!
    return QSize (400,50);
}

//-------------------------------
// minimumSizeHint
//-------------------------------
QSize CheckBoxHeader::minimumSizeHint() const
{
    return QSize(200,29);
}


//===============================
// Implementations of NameIconHeader
//===============================
//-------------------------------
// Konstruktor
//-------------------------------
NameIconHeader::NameIconHeader(Qt::Orientation orientation, QWidget* parent /*= 0*/)
    : QHeaderView(orientation, parent)
{
    setMinimumSectionSize(70);
}

//-------------------------------
// paintSection draws the item
//-------------------------------
void NameIconHeader::paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const
{
    if (!rect.isValid())
        return;
    // get the state of the section
    QStyleOptionHeader opt;
    initStyleOption(&opt);
    QStyle::State state = QStyle::State_None;
    if (isEnabled())
        state |= QStyle::State_Enabled;
    if (window()->isActiveWindow())
        state |= QStyle::State_Active;

    if (isSortIndicatorShown() && sortIndicatorSection() == logicalIndex)
        opt.sortIndicator = (sortIndicatorOrder() == Qt::AscendingOrder)
                            ? QStyleOptionHeader::SortDown : QStyleOptionHeader::SortUp;

    // setup the style options structure
    QVariant textAlignment = model()->headerData(logicalIndex, Qt::Vertical,
                                                  Qt::TextAlignmentRole);
    opt.rect = rect;
    opt.section = logicalIndex;
    opt.state |= state;
    opt.textAlignment = Qt::AlignLeft;

    opt.iconAlignment = Qt::AlignVCenter;
    opt.text = model()->headerData(logicalIndex, Qt::Vertical,
                                    Qt::DisplayRole).toString();

    int margin = 2 * style()->pixelMetric(QStyle::PM_HeaderMargin, 0, this);

    const Qt::Alignment headerArrowAlignment = static_cast<Qt::Alignment>(style()->styleHint(QStyle::SH_Header_ArrowAlignment, 0, this));
    const bool isHeaderArrowOnTheSide = headerArrowAlignment & Qt::AlignVCenter;
    if (isSortIndicatorShown() && sortIndicatorSection() == logicalIndex && isHeaderArrowOnTheSide)
        margin += style()->pixelMetric(QStyle::PM_HeaderMarkSize, 0, this);

    const QVariant variant = model()->headerData(logicalIndex, Qt::Vertical,
                                                  Qt::DecorationRole);
    opt.icon = qvariant_cast<QIcon>(variant);
    if (opt.icon.isNull())
        opt.icon = qvariant_cast<QPixmap>(variant);
    if (!opt.icon.isNull()) // see CT_HeaderSection
        margin += style()->pixelMetric(QStyle::PM_SmallIconSize, 0, this) +
                  style()->pixelMetric(QStyle::PM_HeaderMargin, 0, this);

    //if (d->textElideMode != Qt::ElideNone)
      //  opt.text = opt.fontMetrics.elidedText(opt.text, d->textElideMode , rect.width() - margin);

    QVariant foregroundBrush = model()->headerData(logicalIndex, Qt::Vertical,
                                                    Qt::ForegroundRole);
    if (foregroundBrush.canConvert<QBrush>())
        opt.palette.setBrush(QPalette::ButtonText, qvariant_cast<QBrush>(foregroundBrush));

    QPointF oldBO = painter->brushOrigin();
    QVariant backgroundBrush = model()->headerData(logicalIndex, Qt::Vertical,
                                                    Qt::BackgroundRole);
    if (backgroundBrush.canConvert<QBrush>()) {
        opt.palette.setBrush(QPalette::Button, qvariant_cast<QBrush>(backgroundBrush));
        opt.palette.setBrush(QPalette::Window, qvariant_cast<QBrush>(backgroundBrush));
        painter->setBrushOrigin(opt.rect.topLeft());
    }

    // the section position
    int visual = visualIndex(logicalIndex);
    Q_ASSERT(visual != -1);

    bool first = (logicalIndex == 0);
    bool last = (logicalIndex == model()->rowCount());
    if (first && last)
        opt.position = QStyleOptionHeader::OnlyOneSection;
    else if (first)
        opt.position = false ? QStyleOptionHeader::End : QStyleOptionHeader::Beginning;
    else if (last)
        opt.position = false ? QStyleOptionHeader::Beginning : QStyleOptionHeader::End;
    else
        opt.position = QStyleOptionHeader::Middle;
    opt.orientation = Qt::Vertical;
    // the selected position
    bool previousSelected = false; //d->isSectionSelected(this->logicalIndex(visual - 1));
    bool nextSelected =  false; //d->isSectionSelected(this->logicalIndex(visual + 1));
    if (previousSelected && nextSelected)
        opt.selectedPosition = QStyleOptionHeader::NextAndPreviousAreSelected;
    else if (previousSelected)
        opt.selectedPosition = QStyleOptionHeader::PreviousIsSelected;
    else if (nextSelected)
        opt.selectedPosition = QStyleOptionHeader::NextIsSelected;
    else
        opt.selectedPosition = QStyleOptionHeader::NotAdjacent;
    // draw the section
    style()->drawControl(QStyle::CE_Header, &opt, painter, this);

    painter->setBrushOrigin(oldBO);
    /*QStyleOptionHeader opt;
    initStyleOption(&opt);
    opt.state |= (QStyle::State_Enabled | QStyle::State_Active);
    opt.rect = rect;
    opt.position = QStyleOptionHeader::Beginning;
    opt.section = logicalIndex;
    opt.orientation = Qt::Vertical;
    opt.text = "Hallo";
    style()->drawControl(QStyle::CE_Header, &opt, painter, this);*/


}

//-------------------------------
// viewportEvent looks for tooltips and shows them
//-------------------------------
void NameIconHeader::mouseMoveEvent(QMouseEvent *e)
{
    QHeaderView::mouseMoveEvent(e);
    int logical = logicalIndexAt(e->pos());
    if (logical != -1)
    {
        QVariant variant = model()->headerData(logical, orientation(), Qt::ToolTipRole);
        if (variant.isValid())
        {
            QToolTip::showText(e->globalPos(), variant.toString(), this);
        }
        else
            QToolTip::hideText();
    }
}

//-------------------------------
// mousePressEvent
//-------------------------------
void NameIconHeader::mousePressEvent(QMouseEvent* event)
{}
