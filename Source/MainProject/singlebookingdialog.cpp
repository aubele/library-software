#include "singlebookingdialog.h"
#include "ui_singlebookingdialog.h"
#include "delegates.h"
#include <damagedialog.h>
#include <QDate>

//--------------------------------
// Konstructor
//--------------------------------
SingleBookingDialog::SingleBookingDialog(StudentTable::StudentRow *row, StudentTable* stable, BookTable* btable, LendingTable* ltable, QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    ui(new Ui::SingleBookingDialog)
{
    ui->setupUi(this);
    connect(ui->button_close, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->comboBox_bookingoptions, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBox_bookingoptions_currentIndex_changed(int)));
    connect(ui->radioButton_holidayLending, SIGNAL(toggled(bool)), this, SLOT(radioButton_changed(bool)));
    mModel = new ShortBookModel(row, stable, btable, ltable, this);
    ui->tableView->setModel(mModel);
    ui->tableView->setHorizontalHeader(new QHeaderView(Qt::Horizontal, ui->tableView));
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setItemDelegate(new LendDelegate(ui->tableView));
    mModel->select(ShortBookModel::BooksForClass);
    styleWindow();
}

//------------------------------------
// styleWindow styles the window based on the cfg
//------------------------------------
void SingleBookingDialog::styleWindow()
{
    /*Config *cfg = ConfigCache::getConfig("liborg.config");
    if (cfg)
    {
        QColor brightcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Dark", VariousValue(VariousValue::RGB_Color, "177,216,228")).asColor();
        QColor darkcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Light", VariousValue(VariousValue::RGB_Color, "240,253,253")).asColor();

    }*/
}

//--------------------------------
// Destructor
//--------------------------------
SingleBookingDialog::~SingleBookingDialog()
{
    delete ui;
}

//---------------------------------------
// comboBox_bookingoptions_currentIndex_changed called when a new item is selected
//---------------------------------------
void SingleBookingDialog::comboBox_bookingoptions_currentIndex_changed(int index)
{
    ShortBookModel::option option;
    switch(index)
    {
    case 0:
        option = ShortBookModel::BooksForClass;
        break;
    case 1:
        option = ShortBookModel::AllBooks;
        break;
    case 2:
        option = ShortBookModel::LendBooks;
        break;
    default:
        return;
    }
    mModel->select(option);
}

//---------------------------------------
// radioButton_changed called when the status of the radioButton for holidaylendings changed
//---------------------------------------
void SingleBookingDialog::radioButton_changed(bool value)
{
    mModel->setHolidayLending(value);
}


//=======================================
// Implementations of ShortBookModel
//=======================================
//---------------------------------------
// Konstructor
//---------------------------------------
ShortBookModel::ShortBookModel(StudentTable::StudentRow *row, StudentTable* stable, BookTable* btable, LendingTable* ltable, QWidget *parent):
    QAbstractTableModel(parent),
    mSTable(stable),mBTable(btable), mLTable(ltable), mHolidayLending(false), mOption(ShortBookModel::BooksForClass)
{
    mStudent.comment = row->comment;
    mStudent.dateofbirth = row->dateOfBirth;
    mStudent.forename = row->preName;
    mStudent.surname = row->surName;
    mStudentRow = row;
}

//---------------------------------------
// Destructor
//---------------------------------------
ShortBookModel::~ShortBookModel()
{
    mSTable = nullptr; mBTable = nullptr; mLTable = nullptr; mStudentRow = nullptr;
}

//----------------------------
// rowCount returns the number of rows
//---------------------------
int ShortBookModel::rowCount(const QModelIndex &parent ) const
{
    Q_UNUSED(parent);
    return mBooks.size();
}

//-----------------------------
// columnCount returns the number of columns (3)
//-----------------------------
int ShortBookModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

//-----------------------------
// headerData returns the values for each header
//-----------------------------
QVariant ShortBookModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch(section)
        {
        case 0:
            return "ausgewählt";
        case 1:
            return "Titel";
        case 2:
            return "Untertitel";
        }
    }

    return QVariant();
}

//-----------------------------
// data returns the information for each index of the table
//-----------------------------
QVariant ShortBookModel::data(const QModelIndex &index, int role) const
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
            case 1:
                return st.title;
            case 2:
                return st.subtitle;
             default:
                return QVariant();
            }
        }
        case Qt::CheckStateRole:
        {
            if (index.column() > 0)
                return QVariant();
            if (mStudent.lendings.contains(mBooks.at(index.row()).isbn))
                return mStudent.lendings[mBooks.at(index.row()).isbn].id.size();
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
// lendNewBook
// Attention: reference parameter!
//---------------------------
bool ShortBookModel::lendNewBook(BookTable::BookRow book, Student &student)
{
    int id;
    LendingTable::LendingType type = LendingTable::NORMAL;
    if (mHolidayLending)
        type = LendingTable::HOLIDAY;

    if(!mLTable->insertLending(&id, 0, student.forename,student.surname,
                                          student.dateofbirth,
                                          book.isbn, type,
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
            mSTable->updateStudentComment(student.forename,student.surname, student.dateofbirth,student.comment);
            mStudentRow->comment = student.comment;
        }
    }
    else
    {
        LendInfo info;
        info.id.append(id);
        student.lendings.insert(book.isbn, info);
    }

    // update the available amount of the book
    BookTable::BookRow *row = mBTable->getBook(book.isbn);
    if (row)
    {
        int amount = row->available;
        mBTable->updateAvailable(book.isbn, amount - 1);
    }

    return true;
}

//---------------------------
// giveBackBook
// Attention: student is a reference parameter
//---------------------------
bool ShortBookModel::giveBackBook(BookTable::BookRow book, Student &student, QString damage)
{
    bool rueckgabe = false;
    LendingTable::LendingRow * togivebackrow = mLTable->getLending(student.lendings[book.isbn].id.at(0));
    QDate lastReturnDate = QDate::fromString(togivebackrow->LendDate, "dd.MM.yyyy");

    for (int i = 1; i < student.lendings[book.isbn].id.size(); i++)
    {
        LendingTable::LendingRow * row = mLTable->getLending(student.lendings[book.isbn].id.at(i));
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
    if (mLTable->insertGiveBack(togivebackrow))
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
                mSTable->updateStudentComment(student.forename,student.surname, student.dateofbirth,student.comment);
                mStudentRow->comment = student.comment;
            }
        }
        rueckgabe = true;
    }
    delete togivebackrow;

    // update the available number of the book
    if (rueckgabe)
    {
        BookTable::BookRow *row = mBTable->getBook(book.isbn);
        if (row)
        {
            int amount = row->available;
            mBTable->updateAvailable(book.isbn, amount + 1);
        }
    }

    return rueckgabe;
}

//---------------------------
// setData sets the data on the model that means
// for role = Qt::CheckStateRole and value == 1 add a booking and for value == -1 remove last booking
//---------------------------
bool ShortBookModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
            return false;

        if (role != Qt::CheckStateRole)
            return false;

        if (index.column() > 0)
            return false;

        if (value.toInt() == 1)
        {
            // add a new booking
            BookTable::BookRow book = mBooks.at(index.row());
            if (!lendNewBook(book, mStudent))
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
            BookTable::BookRow book = mBooks.at(index.row());
            if (!mStudent.lendings.contains(book.isbn))
                return false;

            DamageDialog dialog(mStudent.forename, mStudent.surname, book.title);
            if (dialog.exec() != QDialog::Accepted)
                return false;

            if(giveBackBook(book,mStudent, dialog.getDamage()))
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

//--------------------------------
// flags returns the flags for each item
//--------------------------------
Qt::ItemFlags ShortBookModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    if (index.column() == 0)
        return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable;
    else
        return QAbstractTableModel::flags(index);
}

//---------------------------
// select filters the books according to the chosen option
//---------------------------
void ShortBookModel::select(ShortBookModel::option option)
{
    mOption = option;

    beginResetModel();
    mBooks.clear();
    QList<ClassTable::ClassRow> classes = mSTable->getClassesFromStudent(mStudent.forename, mStudent.surname, mStudent.dateofbirth);

    QList<BookTable::BookRow> rows;
    switch(mOption)
    {
    case AllBooks:
    {
        mBooks = mBTable->getAllBooks();
        break;
    }
    case BooksForClass:
    {
        ClassTable::ClassRow crow;
        QHash<QString, char> bookHash;

        foreach (crow, classes)
        {
            rows = mBTable->getBooksByGrade(crow.grade);
            BookTable::BookRow row;
            foreach(row, rows)
            {
                if (!bookHash.contains(row.isbn))
                {
                    mBooks.append(row);
                    bookHash.insert(row.isbn, 'c');
                }
            }
        }
        break;
    }
    case LendBooks:
    {
        QList<LendingTable::LendingRow> lrows = mLTable->getOpenLendingsFromStudent(mStudent.forename,
                                     mStudent.surname , mStudent.dateofbirth);
        LendingTable::LendingRow lrow;
        foreach(lrow, lrows)
        {
            BookTable::BookRow *brow = mBTable->getBook(lrow.BookIsbn);
            if(mHolidayLending)
            {
                if (lrow.type == LendingTable::HOLIDAY)
                    mBooks.append(*brow);
            }
            else
                mBooks.append(*brow);

            delete brow;
        }
        break;
    }
    }

    mStudent.lendings.clear();
    QList<LendingTable::LendingRow> lrows = mLTable->getOpenLendingsFromStudent(mStudent.forename,
                                 mStudent.surname , mStudent.dateofbirth);
    LendingTable::LendingRow lrow;
    foreach(lrow, lrows)
    {
        if (mStudent.lendings.contains(lrow.BookIsbn))
            mStudent.lendings[lrow.BookIsbn].id.append(lrow.ID);
        else
        {
            LendInfo info; info.id.append( lrow.ID);
            mStudent.lendings.insert(lrow.BookIsbn, info);
        }
    }

    endResetModel();
}

//--------------------------------
// setHolidayLending sets whether the bookings are for holiday or not
//--------------------------------
void ShortBookModel::setHolidayLending(bool value)
{
    mHolidayLending = value;
    if (mOption == LendBooks)
        select(mOption);
}
