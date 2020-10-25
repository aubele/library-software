#include "selectclass.h"
#include "ui_selectclass.h"
#include <addclass.h>
#include <delegates.h>

#include "QSortFilterProxyModel"

//===================================
// Implementation of SelectClass
//===================================

//------------------------------------
// Konstructor
//------------------------------------
SelectClass::SelectClass(Dataset* set, QList<ClassTable::ClassRow> classes, QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    mDataset(set),
    ui(new Ui::SelectClass)
{

    ui->setupUi(this);

    //connect the Buttons to the functions
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->pushButton_addClass, SIGNAL(clicked()), this, SLOT(newClass()));
    connect(ui->pushButton_apply, SIGNAL(clicked()), this, SLOT(accept()));

    mModel = new ClassModel(mDataset, this);

    mModel->fill();
    mModel->selectClasses(classes);
    setupTableView();
    styleWindow();
}

//------------------------------------
// styleWindow styles the window based on the cfg
//------------------------------------
void SelectClass::styleWindow()
{
    /*Config *cfg = ConfigCache::getConfig("liborg.config");
    if (cfg)
    {
        QColor brightcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Dark", VariousValue(VariousValue::RGB_Color, "177,216,228")).asColor();
        QColor darkcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Light", VariousValue(VariousValue::RGB_Color, "240,253,253")).asColor();

    }*/
}

//--------------------------
// Destructor
//--------------------------
SelectClass::~SelectClass()
{
    delete ui;
}

//--------------------------
// setupTableView constructs the tableview
//--------------------------
void SelectClass::setupTableView()
{
    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(mModel);
    ui->tableView_classes->setModel(proxyModel);

    ui->tableView_classes->setItemDelegate(new CheckBoxDelegate(ui->tableView_classes));
    ui->tableView_classes->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // If you want sorting per click on header
    //ui->tableView_classes->setSortingEnabled(true);
    ui->tableView_classes->sortByColumn(2, Qt::AscendingOrder);
}

//---------------------------
// newClass called when the button to add a class is clicked
//---------------------------
void SelectClass::newClass()
{
    addclass aclass(this);
    if(aclass.exec() == QDialog::Accepted)
    {
        mModel->addClass(aclass.getNewClass());
    }
}

//--------------------------
// getSelectedClasses returns the selected classes
//--------------------------
QList<ClassTable::ClassRow> SelectClass::getSelectedClasses()
{
    return mModel->getSelectedClasses();
}
