#include "comboboxmodel.h"

bool ClassLessThan(const QStandardItem* s1, const QStandardItem* s2)
{
    int grade1;
    QString s = s1->data(Qt::UserRole).typeName();
    if (QString(s1->data(Qt::UserRole).typeName()) == "QVariantList")
        grade1 = s1->data(Qt::UserRole).toList().at(0).toInt();
    else
        grade1 = s1->data(Qt::UserRole).toInt();

    QString text1 = s1->text();
    int grade2;
    if (QString(s2->data(Qt::UserRole).typeName()) == "QVariantList")
        grade2 = s2->data(Qt::UserRole).toList().at(0).toInt();
    else
        grade2 = s2->data(Qt::UserRole).toInt();
    QString text2 = s2->text();

    if (grade1 < grade2)
        return true;
    if (grade1 > grade2)
        return false;

    if (text1 < text2)
        return true;
    else
        return false;
}

ComboBoxModel::ComboBoxModel(QObject *parent) :
    QStandardItemModel(parent)
{}
ComboBoxModel::~ComboBoxModel()
{}

void ComboBoxModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column);
    if(order != Qt::AscendingOrder)
        return;

    QList<QStandardItem*> items;
    int rowcount = rowCount();
    for (int row = 0; row < rowcount; ++row)
    {
        items.append( takeItem(row, 0));
    }
    this->clear();
    qStableSort(items.begin(), items.end(), ClassLessThan);
    foreach(QStandardItem* item, items)
        this->appendRow(item);
}
