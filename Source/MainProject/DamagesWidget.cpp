#include "DamagesWidget.h"
#include "ui_DamagesWidget.h"
#include <QWidgetItem>
#include <QMessageBox>

//==============================
// Implementations of DamagesWidget
//==============================

//------------------------------
// Constructor
//------------------------------
DamagesWidget::DamagesWidget(Dataset* set, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DamagesWidget), mDataset(set)
{
    ui->setupUi(this);
    connect(ui->pushButton_Close, SIGNAL(clicked()), this, SIGNAL(closewidget()));
    setupTableWidget();
    styleWindow();
}

//------------------------------
// Destructor
//------------------------------
DamagesWidget::~DamagesWidget()
{
    delete ui;
    mDataset = nullptr;
}

//------------------------------------
// styleWindow styles the window based on the cfg
//------------------------------------
void DamagesWidget::styleWindow()
{
    /*Config *cfg = ConfigCache::getConfig("liborg.config");
    if (cfg)
    {
        QColor brightcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Dark", VariousValue(VariousValue::RGB_Color, "177,216,228")).asColor();
        QColor darkcolor = cfg->getValueForKeyInBlock("Einstellungen", "Background-Light", VariousValue(VariousValue::RGB_Color, "240,253,253")).asColor();

    }*/
}

//------------------------------------
// setupTableWidget sets up the qtreewidget
//------------------------------------
void DamagesWidget::setupTableWidget()
{
    // get all items with damage
    QList<LendingTable::LendingRow> list = mDataset->mLendTable.getLendingsWithDamage();
    foreach(LendingTable::LendingRow row, list)
    {
        QString name= row.StudentSurName + ", " + row.StudentForeName + ", " + row.StudentBirthday;
        QTreeWidgetItem *parentItem = nullptr;
        if (ui->treeWidget->findItems(name, Qt::MatchExactly).length() == 0)
        {
            QStringList slist;
            slist.append(name);
            parentItem = new QTreeWidgetItem (ui->treeWidget, slist);
            ui->treeWidget->addTopLevelItem(parentItem);
        }
        else // here it is known that the treewidget contains an item
        {
            parentItem = ui->treeWidget->findItems(name, Qt::MatchExactly).at(0);
        }
        QStringList slist;
        slist.append("");
        BookTable::BookRow *brow = mDataset->mBookTable.getBook(row.BookIsbn);
        if (!brow)
            continue;
        slist.append(brow->title);
        slist.append(row.GiveBackRow.Damage);
        QTreeWidgetItem *child = new QTreeWidgetItem(parentItem, slist);
        child->setData(0, Qt::UserRole, row.GiveBackRow.ID);
        parentItem->addChild(child);
        TreeWidgetItemButton* button = new TreeWidgetItemButton("Begleichen", child, ui->treeWidget);
        connect(button, SIGNAL(clicked(QTreeWidgetItem*)), this, SLOT(button_s_clicked(QTreeWidgetItem*)));
        ui->treeWidget->setItemWidget(child, 3, button);
    }
    for (int i = 0; i < 4; i++)
        ui->treeWidget->resizeColumnToContents(i);
}

//-----------------------
// button_s_clicked called by each button of the qtreewidgetitems
//-----------------------
void DamagesWidget::button_s_clicked(QTreeWidgetItem* item)
{
    ui->treeWidget->setCurrentItem(item);

    QTreeWidgetItem* parent = item->parent();
    int id = item->data(0, Qt::UserRole).toInt();
    QMessageBox box(QMessageBox::Question, "Begleichen?", "Ist der Schaden an dem Buch beglichen?", QMessageBox::NoButton, this);
    QAbstractButton* pButtonYes = (QAbstractButton*)box.addButton("Ja", QMessageBox::AcceptRole);
    box.addButton("Nein", QMessageBox::RejectRole);
    box.exec();
    if (box.clickedButton() == pButtonYes)
    {
        mDataset->mLendTable.updateGiveBackDamage(id, "");
        parent->removeChild(item);
        if (parent->childCount() == 0)
            delete parent;
    }
}
