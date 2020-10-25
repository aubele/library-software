#include "bookingwidget.h"
#include "ui_bookingwidget.h"
#include <QStandardItemModel>
#include "comboboxmodel.h"

BookingWidget::BookingWidget(Dataset *set, QString *lastselectedClass, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BookingWidget),
    mDataset(set), mView(nullptr), mLastClass(lastselectedClass)
{
    ui->setupUi(this);
    connect(ui->button_close, SIGNAL(clicked()), this, SIGNAL(closewidget()));

    // Get all available classes and set the current index invalid
    ComboBoxModel *model = new ComboBoxModel(this);
    ui->comboBox_class->setModel(model);
    QList<ClassTable::ClassRow> rows = set->mClassTable.getAllClasses();
    ClassTable::ClassRow row;
    foreach(row, rows)
    {
        QList<QVariant> list;
        list.append(row.grade);
        list.append(row.subject);
        ui->comboBox_class->addItem(row.className, list);
    }
    model->sort(0, Qt::AscendingOrder);
    ui->comboBox_class->setCurrentIndex(-1);
    connect(ui->comboBox_class, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBox_currentIndex_changed()));
    connect(ui->comboBox_bookingoptions, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBox_currentIndex_changed()));
    connect(ui->searchline, SIGNAL(returnPressed()), ui->searchbutton, SIGNAL(clicked()));
    connect(ui->searchbutton, SIGNAL(clicked()), this, SLOT(search()));
    mModel = new StudentLendModel(&mDataset->mStudentTable, &mDataset->mBookTable, &mDataset->mLendTable, this->fontMetrics(), this);
    styleWindow();
    if (!lastselectedClass->isEmpty())
        ui->comboBox_class->setCurrentText(*lastselectedClass);
}

//------------------------------------
// styleWindow styles the window based on the cfg
//------------------------------------
void BookingWidget::styleWindow()
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
BookingWidget::~BookingWidget()
{
    delete ui;
    mDataset = nullptr;
    mView = nullptr;
}

//---------------------------------------
// comboBox_class_currentIndex_changed called when a new item is selected
//---------------------------------------
void BookingWidget::comboBox_currentIndex_changed()
{
    int index = ui->comboBox_class->currentIndex();
    if(index == -1)
        return;
    ui->searchline->setText("");
    QList<QVariant> data = ui->comboBox_class->itemData(index).toList();
    int grade = data.at(0).toInt();
    QString subject = data.at(1).toString();
    subject.replace(" spätbeginnend", "");
    QString classname = ui->comboBox_class->itemText(index);
    *mLastClass = classname;
    if (mView == nullptr)
    {
        mView = new QTableView(ui->widget_booking);
        ui->widget_booking->layout()->addWidget(mView);
        ui->widget_booking->layout()->setSpacing(0);
        ui->widget_booking->layout()->setMargin(0);
        mView->setModel(mModel);
        ui->label_select_class->hide();
        ui->widget_booking->layout()->removeWidget(ui->label_select_class);
        mView->setItemDelegate(new ItemDelegate(mView));
        mView->setHorizontalHeader(new CheckBoxHeader(Qt::Horizontal, mView));
        mView->setVerticalHeader(new NameIconHeader(Qt::Vertical, mView));
    }
    StudentLendModel::BookOption option = get_current_comboBox_bookingoption();
    mModel->selectClass(classname, grade, subject, option);
    for (int i = 0; i < mModel->columnCount(QModelIndex()); i++)
    {
        int width = 0;
        if (i == 0)
            width = 50;
        else if (i == 1)
            width = 200;
        else
            width = 250;
        //mView->setColumnWidth(i, width);
    }
    //amount of students in the class
    ui->label_studentcount->setText(QString::number(mDataset->mStudentTable.getStudentsByClassName(classname).count()));
}

//---------------------------------------
// get_current_comboBox_bookingoption returns the current selected bookingoption
//---------------------------------------
StudentLendModel::BookOption BookingWidget::get_current_comboBox_bookingoption()
{
    int index = ui->comboBox_bookingoptions->currentIndex();
    StudentLendModel::BookOption option;
    if (index == 0)
        option = StudentLendModel::BooksForClass;
    else if (index == 1)
        option = StudentLendModel::LentBooks;
    else
        option = StudentLendModel::AllBooks;
   return option;
}

//---------------------------------------
// search called when the searchbutton is triggered
//---------------------------------------
void BookingWidget::search()
{
    if (ui->comboBox_class->currentText().isEmpty())
        return;
    QString searchstring = ui->searchline->text();
    StudentLendModel::BookOption option = get_current_comboBox_bookingoption();
    mModel->searchInBooks(searchstring, option);
    for (int i = 0; i < mModel->columnCount(QModelIndex()); i++)
    {
        int width = 0;
        if (i == 0)
            width = 50;
        else if (i == 1)
            width = 200;
        else
            width = 250;
        //mView->setColumnWidth(i, width);
    }
}
