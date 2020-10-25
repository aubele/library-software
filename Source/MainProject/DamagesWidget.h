#ifndef DAMAGESWIDGET_H
#define DAMAGESWIDGET_H

#include <QWidget>
#include "dataset.h"
#include <QTreeWidgetItem>
#include <QPushButton>

namespace Ui {
class DamagesWidget;
}

class DamagesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DamagesWidget(Dataset* set, QWidget *parent = 0);
    ~DamagesWidget();

private:
    Ui::DamagesWidget *ui;
    Dataset* mDataset;
    void styleWindow();

private slots:
    void setupTableWidget();
    void button_s_clicked(QTreeWidgetItem* item);

signals:
    void closewidget();
};

class TreeWidgetItemButton : public QPushButton
{
    Q_OBJECT

public:
    TreeWidgetItemButton(QString text, QTreeWidgetItem* item, QWidget* parent) :
        QPushButton(text, parent), mItem(item)
    {
        connect(this, SIGNAL(clicked()), this, SLOT(button_clicked()));
    }

    ~TreeWidgetItemButton() {}

signals:
    void clicked(QTreeWidgetItem* item);

private:
    QTreeWidgetItem* mItem;

private slots:
    void button_clicked()
    {
        emit clicked(mItem);
    }
};

#endif // DAMAGESWIDGET_H
