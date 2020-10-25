#include "OpenLendings.h"
#include "ui_OpenLendings.h"
#include <QMenu>
#include "singlebookingdialog.h"

//=================================
// Implementation of OpenLendings
//=================================
OpenLendings::OpenLendings(Dataset* dataset, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OpenLendings), mDataset(dataset)
{
    ui->setupUi(this);
    connect(ui->pushButton_Close, SIGNAL(clicked()), this, SIGNAL(closewidget()));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBox_currentIndex_changed(int)));
    mModel = new LendBooksModel(mDataset, this);
    setupTableView();
    styleWindow();
}

//--------------------------------
// Destructor
//--------------------------------
OpenLendings::~OpenLendings()
{
    delete ui;
    mDataset = nullptr;
}

//--------------------------------
// styleWindow does nothing yet
//--------------------------------
void OpenLendings::styleWindow()
{
    /*Config *cfg = ConfigCache::getConfig("liborg.config");
    if (cfg)
    {
        QColor brightcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Dark", VariousValue(VariousValue::RGB_Color, "177,216,228")).asColor();
        QColor darkcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Light", VariousValue(VariousValue::RGB_Color, "240,253,253")).asColor();

    }*/
}

//--------------------------------
// setupTableView
//--------------------------------
void OpenLendings::setupTableView()
{
    ui->tableView->setModel(mModel);
    // context menu
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect( ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenuRequested(QPoint)));
    connect( ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(rowDoubleClicked(QModelIndex)));
    // stretch
    ui->tableView->setHorizontalHeader(new QHeaderView(Qt::Horizontal, ui->tableView));
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui->tableView->setItemDelegate(new ItemDelegateStudent(ui->tableView));
    // selection
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mModel->selectOption("alle Ausleihen");
}

//--------------------------------
// comboBox_currentIndex_changed called by the combobox if the index has changed
//--------------------------------
void OpenLendings::comboBox_currentIndex_changed(int index)
{
    Q_UNUSED(index);
    mModel->selectOption(ui->comboBox->currentText());
}

//---------------------------------------
// customMenuRequested(): called when a context-menu should be shown
//---------------------------------------
void OpenLendings::customMenuRequested(QPoint pos)
{
    QModelIndex index = ui->tableView->indexAt(pos);

    if(index.row() < 0)
        return;

    QMenu* menu = new QMenu(this);
    QAction* bookAct = new QAction("Ausleihe und Rückgabe", this);
    connect(bookAct, SIGNAL(triggered()), this, SLOT(singlebooking()));
    menu->addAction(bookAct);

    menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

//---------------------------------------
// singlebooking(): called by the context menu; opens a dialog for bookings
//---------------------------------------
void OpenLendings::singlebooking()
{
    QModelIndex index = ui->tableView->selectionModel()->currentIndex();
    StudentTable::StudentRow* row = mModel->getStudent(index);
    SingleBookingDialog dialog(row, &mDataset->mStudentTable, &mDataset->mBookTable, &mDataset->mLendTable, this);
    dialog.exec();
    mModel->updateRow(index);
}

//---------------------------------------
// rowDoubleClicked called when a row is double clicked -> open singlebooking
//---------------------------------------
void OpenLendings::rowDoubleClicked(QModelIndex index)
{
    Q_UNUSED(index);
    singlebooking();
}


//=======================================
// Implementations of LendBooksModel
//=======================================

//---------------------------------------
// Constructor
//---------------------------------------
LendBooksModel::LendBooksModel(Dataset *set, QObject* parent) :
    QAbstractTableModel(parent), mDataset(set), mReturnRow(nullptr)
{
}

//---------------------------------------
// Destructor
//---------------------------------------
LendBooksModel::~LendBooksModel()
{
    mDataset = nullptr;
    delete mReturnRow;
}

//---------------------------------------
// selectOption loads the values from the database and stores them in the model
//---------------------------------------
void LendBooksModel::selectOption(QString option)
{
    emit beginResetModel();
    mStudents.clear(); mDoubleLendings.clear(); mHolidayLendings.clear(); mAllLendings.clear();
    mStudents = mDataset->mStudentTable.getAllStudents();
    foreach(StudentTable::StudentRow srow, mStudents)
    {
        // get the lending information
        QList<LendingTable::LendingRow> rows =  mDataset->mLendTable.getOpenLendingsFromStudent(
                    srow.preName, srow.surName, srow.dateOfBirth);
        int newDoubleLendings = 0;
        int newAllLendings = rows.length();
        QHash<QString, int> map;
        int newHolidayLendings = 0;

        foreach(LendingTable::LendingRow row, rows)
        {
            if (row.type == LendingTable::HOLIDAY)
                newHolidayLendings++;
            if (!map.contains(row.BookIsbn))
            {
                map.insert(row.BookIsbn, 0);
            }
            else if (map[row.BookIsbn] != 1)
            {
                map[row.BookIsbn] = 1;
                newDoubleLendings++;
            }
        }

        if (option == "alle Ausleihen")
        {
            if (newAllLendings == 0)
                mStudents.removeAll(srow);
            else
            {
                mDoubleLendings.append(newDoubleLendings);
                mHolidayLendings.append(newHolidayLendings);
                mAllLendings.append(newAllLendings);
            }
        }
        else if (option == "Doppelte Ausleihen")
        {
            if (newDoubleLendings == 0)
                mStudents.removeAll(srow);
            else
            {
                mDoubleLendings.append(newDoubleLendings);
                mHolidayLendings.append(newHolidayLendings);
                mAllLendings.append(newAllLendings);
            }
        }
        else if (option == "Ferienausleihen")
        {
            if (newHolidayLendings == 0)
                mStudents.removeAll(srow);
            else
            {
                mDoubleLendings.append(newDoubleLendings);
                mHolidayLendings.append(newHolidayLendings);
                mAllLendings.append(newAllLendings);
            }
        }
    }
    emit endResetModel();
}

//---------------------------------------
// getStudent returns the selected student
//---------------------------------------
StudentTable::StudentRow* LendBooksModel::getStudent(QModelIndex index)
{
    if (mReturnRow)
        delete mReturnRow;
    mReturnRow = new StudentTable::StudentRow;
    StudentTable::StudentRow row = mStudents.at(index.row());
    mReturnRow->address = row.address;
    mReturnRow->classRows = row.classRows;
    mReturnRow->comment = row.comment;
    mReturnRow->dateOfBirth = row.dateOfBirth;
    mReturnRow->preName = row.preName;
    mReturnRow->surName = row.surName;
    return mReturnRow;
}

//---------------------------------------
// updateRow updates the row
//---------------------------------------
void LendBooksModel::updateRow(QModelIndex index)
{
    delete mReturnRow;
    mReturnRow = nullptr;

    QList<LendingTable::LendingRow> rows =  mDataset->mLendTable.getOpenLendingsFromStudent(
                mStudents.at(index.row()).preName, mStudents.at(index.row()).surName,
                mStudents.at(index.row()).dateOfBirth);
    int newDoubleLendings = 0;
    int newAllLendings = rows.length();
    QHash<QString, int> map;
    int newHolidayLendings = 0;

    foreach(LendingTable::LendingRow row, rows)
    {
        if (row.type == LendingTable::HOLIDAY)
            newHolidayLendings++;
        if (!map.contains(row.BookIsbn))
        {
            map.insert(row.BookIsbn, 0);
        }
        else if (map[row.BookIsbn] != 1)
        {
            map[row.BookIsbn] = 1;
            newDoubleLendings++;
        }
    }
    mDoubleLendings.replace(index.row(), newDoubleLendings);
    mHolidayLendings.replace(index.row(), newHolidayLendings);
    mAllLendings.replace(index.row(), newAllLendings);
    emit dataChanged(index, index);
}

//--------------------------
// rowCount
// returns the number of rows
//--------------------------
int LendBooksModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mStudents.count();
}

//---------------------------
// columnCount
// returns the number of columns meaning the number of books
//---------------------------
int LendBooksModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

//---------------------------
// data which returns all informations to display
//---------------------------
QVariant LendBooksModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= mStudents.count())
        return QVariant();

    if (index.column() >= 4)
        return QVariant();

    switch(role)
    {
        case Qt::DisplayRole:
        {
            switch(index.column())
            {
            case 0:
                return mStudents.at(index.row()).surName + ", " + mStudents.at(index.row()).preName;

            case 1:
                return mAllLendings.at(index.row());

            case 2:
                return mDoubleLendings.at(index.row());

            case 3:
                return mHolidayLendings.at(index.row());
            }
        }
        default:
            return QVariant();
    }
}

//---------------------------
// headerData returns all information to display in the header
//---------------------------
QVariant LendBooksModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section >= 4)
        return QVariant();

    if (orientation == Qt::Vertical)
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    switch(section)
    {
    case 0:
        return "Schülername";
    case 1:
        return "Anzahl offene Ausleihen";
    case 2:
        return "Anzahl doppelte Ausleihen";
    case 3:
        return "Anzahl Ferienausleihen";
    }

    return QVariant();
}

//---------------------------
// setData does nothing here and always returns false
//---------------------------
bool LendBooksModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(index); Q_UNUSED(value); Q_UNUSED(role);
    return false;
}

//----------------------------
// flags returns the flag for each cell
//----------------------------
Qt::ItemFlags LendBooksModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);
}
