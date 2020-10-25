#include "bookwidget.h"
#include "ui_bookwidget.h"
#include <QMenu>
#include <QModelIndexList>
#include <QMessageBox>
#include "bookmask.h"

//========================================
// Implementations of BookWidget
//========================================

//----------------------------------------
// Konstructor
//----------------------------------------
BookWidget::BookWidget(Dataset *set,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BookWidget), mDataset(set)
{
    ui->setupUi(this);
    connect(ui->button_close, SIGNAL(clicked()), this, SIGNAL(closewidget()));
    setupTableView();

    // search function
    connect(ui->line_serach, SIGNAL(returnPressed()), ui->button_search, SIGNAL(clicked()));
    connect(ui->button_search, SIGNAL(clicked()), this, SLOT(search()));
    connect(ui->button_addBook, SIGNAL(clicked()), this, SLOT(buttonAddBook_clicked()));

    styleWindow();
}

//------------------------------------
// styleWindow styles the window based on the cfg
//------------------------------------
void BookWidget::styleWindow()
{
    /*Config *cfg = ConfigCache::getConfig("liborg.config");
    if (cfg)
    {
        QColor brightcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Dark", VariousValue(VariousValue::RGB_Color, "177,216,228")).asColor();
        QColor darkcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Light", VariousValue(VariousValue::RGB_Color, "240,253,253")).asColor();

    }*/
}

//----------------------------------------
// setupTableView() initalisation method to setup the tableview
//----------------------------------------
void BookWidget::setupTableView()
{
    // add model
    mModel = new BookModel(&mDataset->mBookTable, &mDataset->mLendTable, this);
    ui->tableView->setModel(mModel);
    // context menu
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect( ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenuRequested(QPoint)));
    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(rowDoubleClicked(QModelIndex)));
    // stretch
    ui->tableView->setHorizontalHeader(new QHeaderView(Qt::Horizontal, ui->tableView));
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui->tableView->setItemDelegate(new ItemDelegateStudent(ui->tableView));
    // selection
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    mModel->search("");
}

//----------------------------------------
// Destructor
//----------------------------------------
BookWidget::~BookWidget()
{
    delete ui;
    mDataset = nullptr;
}

//---------------------------------------
// customMenuRequested(): called when a context-menu should be shown
//---------------------------------------
void BookWidget::customMenuRequested(QPoint pos)
{
    QModelIndex index = ui->tableView->indexAt(pos);

    if(index.row() < 0)
        return;

    QMenu* menu = new QMenu(this);

    QAction* editAct = new QAction("Buch bearbeiten", this);
    connect(editAct, SIGNAL(triggered()), this, SLOT(openBookDialog()));
    menu->addAction(editAct);

    QAction* deleteAct = new QAction("Buch löschen", this);
    connect(deleteAct, SIGNAL(triggered()), this, SLOT(deleteBook()));
    menu->addAction(deleteAct);

    menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

//---------------------------------------
// openBookDialog(): called from the context menu, opens the book dialog
//---------------------------------------
void BookWidget::openBookDialog()
{
    BookMask mask(&mDataset->mBookTable, this);
    QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();
    BookTable::BookRow *row = mModel->getRow(indexes.at(0));
    mask.setBookRow(row);
    if (mask.exec() == QDialog::Accepted)
    {
        mModel->updateRow(indexes.at(0).row());
    }
}

//--------------------------------------
// deleteBook removes the selected book called by the contextmenu "Buch löschen"
//--------------------------------------
void BookWidget::deleteBook()
{
    QModelIndexList list = ui->tableView->selectionModel()->selectedRows();

    if (list.size() == 0)
        return;

    // Setup messagebox
    QMessageBox msgBox;
    msgBox.setWindowTitle("Buch löschen");
    msgBox.setText("Wollen Sie das ausgewählte Buch wirklich löschen?");
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

//---------------------------------------
// buttonAddBook_clicked is called by the buttonAddBook to add a new book
//---------------------------------------
void BookWidget::buttonAddBook_clicked()
{
    BookMask mask(&mDataset->mBookTable, this);
    if (mask.exec() == QDialog::Accepted)
    {
        BookTable::BookRow *newRow = mask.getInsertedBookRow();
        mModel->addRow(newRow);
        delete newRow;
    }
}

//---------------------------------------
// search(): called from the search button or from the search lineedit, when enter is pressed
//---------------------------------------
void BookWidget::search()
{
    QString searchString = ui->line_serach->text();
    mModel->search(searchString);
}

//---------------------------------------
// rowDoubleClicked called when a row is double clicked -> open BookDialog
//---------------------------------------
void BookWidget::rowDoubleClicked(QModelIndex index)
{
    BookMask mask(&mDataset->mBookTable, this);
    BookTable::BookRow *row = mModel->getRow(index);
    mask.setBookRow(row);
    if (mask.exec() == QDialog::Accepted)
    {
        mModel->updateRow(index.row());
    }
}
