#include "pupilbookdialog.h"
#include "ui_pupilbookdialog.h"
#include "../db/studenttable.h"
#include "../db/booktable.h"
#include "../db/lendingtable.h"
#include "lendmodel.h"


//========================================
// Implementations of PupilBookDialog
//========================================

//----------------------------------------
// Constructor
//----------------------------------------
PupilBookDialog::PupilBookDialog(int grade, StudentTable::StudentRow* row, StudentTable* stable, BookTable* btable, LendingTable* ltable, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PupilBookDialog),
    mGrade(grade),
    mRow(row),
    mStudentTable(stable),
    mLendingTable(ltable),
    mBookTable(btable)
{
    ui->setupUi(this);
    studentName = (row->surName + ", " + row->preName);
    ui->label_pupil->setText(studentName);
    books = btable->getBooksByGrade(grade);
    setupTableView();

}

//----------------------------------------
// Destructor
//----------------------------------------
PupilBookDialog::~PupilBookDialog()
{
    delete ui;
}

//----------------------------------------
// setupTableView(): fills the TableView with the bookdata from the student and shows it
//----------------------------------------
void PupilBookDialog::setupTableView()
{
    mModel = new LendModel(mRow, mStudentTable, mBookTable, mLendingTable, this->fontMetrics(), this);
    ui->tableView->setModel(mModel);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setItemDelegate(new ItemDelegate(ui->tableView));
    ui->tableView->horizontalHeader()->hide();
}

