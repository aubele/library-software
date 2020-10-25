#include "studentwidget.h"
#include "ui_studentwidget.h"
#include "studentmask.h"
#include "QMenu"
#include "QMessageBox"
#include "singlebookingdialog.h"
#include "comboboxmodel.h"


//=============================
// Implementations of StudentWidget
//=============================

//---------------------------------------
// Constructor
//---------------------------------------
StudentWidget::StudentWidget(Dataset* set, UserPermissions permission, QString *lastselectedClass, QWidget *parent) :
    QWidget(parent),
    mDataset(set),
    ui(new Ui::StudentWidget),
    mRowToDelete(-1), mPermissions(permission), mLastClass(lastselectedClass)
{
    ui->setupUi(this);
    connect(ui->closeBtn, SIGNAL(clicked()), this, SIGNAL(closewidget()));

    setupTableView();
    setupClassBox();

    // search function
    connect(ui->searchNameComment, SIGNAL(returnPressed()), ui->searchBtn, SIGNAL(clicked()));
    connect(ui->searchAmount, SIGNAL(returnPressed()), ui->searchBtn, SIGNAL(clicked()));
    connect(ui->searchBtn, SIGNAL(clicked()), this, SLOT(search()));

    //add new student
    connect(ui->addstudentBtn, SIGNAL(clicked()), this, SLOT(openStudentDialog()));
    if (!(mPermissions & Adding))
        ui->addstudentBtn->hide();

    styleWindow();
}

//------------------------------------
// styleWindow styles the window based on the cfg
//------------------------------------
void StudentWidget::styleWindow()
{
    /*Config *cfg = ConfigCache::getConfig("liborg.config");
    if (cfg)
    {
        QColor brightcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Dark", VariousValue(VariousValue::RGB_Color, "177,216,228")).asColor();
        QColor darkcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Light", VariousValue(VariousValue::RGB_Color, "240,253,253")).asColor();

    }*/
}

//---------------------------------------
// Destructor
//---------------------------------------
StudentWidget::~StudentWidget()
{
    delete ui;
    mDataset = nullptr;
}

//---------------------------------------
// setupClassBox(): init tableview
//---------------------------------------
void StudentWidget::setupTableView()
{
    // add model
    mModel = new StudentModel(&mDataset->mStudentTable, &mDataset->mLendTable, this);
    ui->tableView->setModel(mModel);
    // context menu
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect( ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenuRequested(QPoint)));
    connect( ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(rowDoubleClicked(QModelIndex)));
    // stretch
    ui->tableView->setHorizontalHeader(new QHeaderView(Qt::Horizontal, ui->tableView));
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setItemDelegate(new ItemDelegateStudent(ui->tableView));
    // selection
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

//---------------------------------------
// setupClassBox(): init classbox
//---------------------------------------
void StudentWidget::setupClassBox()
{
    // First add option for all students
    // Then add all available classes
    ComboBoxModel *model = new ComboBoxModel(this);
    ui->classBox->setModel(model);
    ui->classBox->addItem("Alle");
    QList<ClassTable::ClassRow> rows = mDataset->mClassTable.getAllClasses();
    for(ClassTable::ClassRow row : rows)
    {
        ui->classBox->addItem(row.className, QVariant(row.grade));
    }
    model->sort(0, Qt::AscendingOrder);

    // Connect signal for combobox
    connect(ui->classBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBox_class_currentIndex_changed(int)));

    if (!mLastClass->isEmpty())
        ui->classBox->setCurrentText(*mLastClass);
    else
    {
        // Set the index on 'Alle'
        ui->classBox->setCurrentIndex(0);
        // Call the slot once, since the signal does not trigger
        comboBox_class_currentIndex_changed(0);
    }
}

//---------------------------------------
// comboBox_class_currentIndex_changed(): called when a new item is selected
//---------------------------------------
void StudentWidget::comboBox_class_currentIndex_changed(int index)
{
    QString classname = ui->classBox->itemText(index);
    *mLastClass = classname;
    mModel->selectClass(classname);
    ui->label_amountStudent->setText(QString::number(mModel->rowCount(QModelIndex())));
}

//---------------------------------------
// customMenuRequested(): called when a context-menu should be shown
//---------------------------------------
void StudentWidget::customMenuRequested(QPoint pos)
{
    QModelIndex index = ui->tableView->indexAt(pos);

    if(index.row() < 0)
        return;

    QMenu* menu = new QMenu(this);

    if (mPermissions & Adding)
    {
        QAction* editAct = new QAction("Schüler bearbeiten", this);
        connect(editAct, SIGNAL(triggered()), this, SLOT(editStudent()));
        menu->addAction(editAct);

        mRowToDelete = index.row();
        QAction* deleteAct = new QAction("Schüler löschen", this);
        connect(deleteAct, SIGNAL(triggered()), this, SLOT(deleteStudent()));
        menu->addAction(deleteAct);
    }

    QAction* bookAct = new QAction("Ausleihe und Rückgabe", this);
    connect(bookAct, SIGNAL(triggered()), this, SLOT(singlebooking()));
    menu->addAction(bookAct);

    menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

//---------------------------------------
// openStudentDialog(): called from the add button to add a new student
//---------------------------------------
void StudentWidget::openStudentDialog()
{
    StudentMask mask(mDataset, this);
    if(mask.exec() == QDialog::Accepted)
    {
        mModel->addStudent(mask.getNewStudent());
        ui->label_amountStudent->setText(QString::number(mModel->rowCount(QModelIndex())));
    }
}

//---------------------------------------
// editStudent(): called from the context menu, opens the student dialog
//---------------------------------------
void StudentWidget::editStudent()
{
    StudentMask mask(mDataset, this);
    QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();

    mask.selectStudent(mModel->getStudent(indexes.at(0)));
    if (mask.exec() == QDialog::Accepted)
    {
        mModel->updateRow(indexes.at(0).row());
    }
}

//---------------------------------------
// deleteStudent(): deletes the selected student
//---------------------------------------
void StudentWidget::deleteStudent()
{
    // Setup messagebox
    QMessageBox msgBox;
    msgBox.setWindowTitle("Schüler löschen");
    msgBox.setText("Wollen Sie den ausgewählten Schüler wirklich löschen?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setButtonText(QMessageBox::Cancel, "Abbrechen");
    int ret = msgBox.exec();

    if(ret == QMessageBox::Ok)
    {
        // No row selected
        if(mRowToDelete == -1)
            return;
        // Delete it
        mModel->deleteStudent(mRowToDelete);
        ui->label_amountStudent->setText(QString::number(mModel->rowCount(QModelIndex())));

        QString currentClass = ui->classBox->currentText();
        // Maybe a class got deleted, so we need to set it up new
        ui->classBox->clear();
        ui->classBox->addItem("Alle");
        QList<ClassTable::ClassRow> rows = mDataset->mClassTable.getAllClasses();
        for(ClassTable::ClassRow row : rows)
        {
            ui->classBox->addItem(row.className, QVariant(row.grade));
        }
        ui->classBox->model()->sort(0, Qt::AscendingOrder);
        // When the preselected class still exists, select it again
        if(ui->classBox->findText(currentClass) != -1)
        {
            ui->classBox->setCurrentText(currentClass);
        }
    }
    else if(ret == QMessageBox::Cancel)
    {
        return;
    }
}

//---------------------------------------
// singlebooking(): called by the context menu; opens a dialog for bookings
//---------------------------------------
void StudentWidget::singlebooking()
{
    QModelIndex index = ui->tableView->selectionModel()->currentIndex();
    StudentTable::StudentRow* row = mModel->getStudent(index);
    SingleBookingDialog dialog(row, &mDataset->mStudentTable, &mDataset->mBookTable, &mDataset->mLendTable, this);
    dialog.setWindowTitle("Ausleihen von/für " + row->surName + ", " + row->preName);
    dialog.exec();
    mModel->updateRow(index.row());
}

//---------------------------------------
// searchNameComment(): called from the search button or from one of the two search lineedit, when enter is pressed
//---------------------------------------
void StudentWidget::search()
{
    QString searchStringNameComment = ui->searchNameComment->text();
    QString searchStringAmount = ui->searchAmount->text();
    if (searchStringNameComment.isEmpty() && searchStringAmount.isEmpty())
    {
        QString classname = ui->classBox->currentText();
        mModel->selectClass(classname);
    }
    else
        mModel->search(searchStringNameComment, searchStringAmount);

    ui->label_amountStudent->setText(QString::number(mModel->rowCount(QModelIndex())));
}

//---------------------------------------
// rowDoubleClicked called when a row is double clicked -> open BookDialog
//---------------------------------------
void StudentWidget::rowDoubleClicked(QModelIndex index)
{
    StudentTable::StudentRow* row = mModel->getStudent(index);
    SingleBookingDialog dialog(row, &mDataset->mStudentTable, &mDataset->mBookTable, &mDataset->mLendTable, this);
    dialog.setWindowTitle("Ausleihen von/für " + row->surName + ", " + row->preName);
    dialog.exec();
    mModel->updateRow(index.row());
    /*StudentMask mask(mDataset, this);

    mask.selectStudent(mModel->getStudent(index));
    if (mask.exec() == QDialog::Accepted)
    {
        mModel->updateRow(index.row());
    }*/
}
