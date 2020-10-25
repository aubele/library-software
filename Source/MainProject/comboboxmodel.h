#ifndef COMBOBOXMODEL_H
#define COMBOBOXMODEL_H
#include <QStandardItemModel>

class ComboBoxModel : public QStandardItemModel
{
public:
    ComboBoxModel(QObject *parent = nullptr);
    ~ComboBoxModel();
    void sort(int column, Qt::SortOrder order);
 };

#endif // COMBOBOXMODEL_H
