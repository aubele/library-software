#include "classdialog.h"
#include "ui_classdialog.h"
#include "studentmodel.h"
#include "pupilbookdialog.h"
#include "bookdialog.h"


//================================
// Implementations of ClassDialog
//================================

//--------------------------------
// Constructor
//--------------------------------
ClassDialog::ClassDialog(Dataset* set, QString classname, QWidget *parent) :
    QDialog(parent), classname(classname), mDataset(set), ui(new Ui::ClassDialog)
{
    ui->setupUi(this);
    setupTableView();
    ui->label_Classname->setText(classname);
    connect(ui->pushButton_switchToBooks, &QPushButton::clicked, this, &ClassDialog::openBookDialog);
    ui->pushButton_switchToBooks->setFocusPolicy(Qt::NoFocus);
}

//--------------------------------
// Destructor
//--------------------------------
ClassDialog::~ClassDialog()
{
    delete ui;
}

//--------------------------------
// setupTableView(): fills the TableView with the studentdata and shows it
//--------------------------------
void ClassDialog::setupTableView()
{
    QList <StudentTable::StudentRow> allstudents = mDataset->mStudentTable.getStudentsByClassName(classname);
    mModel = new StudentModel(allstudents, &mDataset->mLendTable, this);
    ui->tableView->setModel(mModel);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect( ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(openStudentBooks(QModelIndex)));
}

//--------------------------------
// openStudentBooks(): opens the Dialog with the books of the selected student
//--------------------------------
void ClassDialog::openStudentBooks(QModelIndex index)
{
    StudentTable::StudentRow* row = mModel->getStudent(index);
    int grade = mDataset->mClassTable.getClass(classname)->grade;
    PupilBookDialog dialog(grade, row, &mDataset->mStudentTable, &mDataset->mBookTable, &mDataset->mLendTable, this);
    dialog.showFullScreen();
    dialog.exec();
}

//--------------------------------
// openBookDialog(): opens the Dialog with the Books for the class
//--------------------------------
void ClassDialog::openBookDialog()
{
    BookDialog dialog(mDataset, classname, this);
    dialog.showFullScreen();
    dialog.exec();
}
