#include "bookdialog.h"
#include "ui_bookdialog.h"
#include "booklendmodel.h"


//================================
// Implementations of bookDialog
//================================

//--------------------------------
// Constructor
//--------------------------------
BookDialog::BookDialog(Dataset* set, QString classname, QWidget *parent) :
    QDialog(parent),
    classname(classname),
    mDataset(set),
    mBookTable(&mDataset->mBookTable),
    mLendTable(&mDataset->mLendTable),
    mStudentTable(&mDataset->mStudentTable),
    mClassTable(&mDataset->mClassTable),
    ui(new Ui::BookDialog)
{
    ui->setupUi(this);
    ui->label_Classname->setText(classname);
    connect(ui->pushButton_goBack, &QPushButton::clicked, this, &BookDialog::goBack);
    ui->pushButton_goBack->setFocusPolicy(Qt::NoFocus);
    setupTableView();

}

//--------------------------------
// Destructor
//--------------------------------
BookDialog::~BookDialog()
{
    delete ui;
}

//-----------------------------
// setupTableView(): fills the TableView with the bookdata and shows it
//-----------------------------
void BookDialog::setupTableView()
{
    students = mStudentTable->getStudentsByClassName(classname);
    mModel = new BookLendModel(classname, students, mBookTable, mLendTable, mStudentTable,
                               mClassTable, this->fontMetrics(), this);
    ui->tableView->setModel(mModel);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setItemDelegate(new BookItemDelegate(ui->tableView));
    ui->tableView->horizontalHeader()->hide();
}

//-----------------------------
// goBack(): closes the Book Dialog -> equal to the Android Back-Button
//-----------------------------
void BookDialog::goBack()
{
    close();
}
