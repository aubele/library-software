#ifndef OPENLENDINGS_H
#define OPENLENDINGS_H

#include <QWidget>
#include "dataset.h"
#include <QAbstractTableModel>

namespace Ui {
class OpenLendings;
}

class LendBooksModel : public QAbstractTableModel
{
public:
    LendBooksModel(Dataset *set, QObject* parent);
    ~LendBooksModel();
    void selectOption(QString option);
    StudentTable::StudentRow* getStudent(QModelIndex index);
    void updateRow(QModelIndex index);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    Dataset *mDataset;
    QList<StudentTable::StudentRow> mStudents;
    QList<int> mDoubleLendings;
    QList<int> mHolidayLendings;
    QList<int> mAllLendings;
    StudentTable::StudentRow* mReturnRow;
};


class OpenLendings : public QWidget
{
    Q_OBJECT

public:
    explicit OpenLendings(Dataset* dataset, QWidget *parent = 0);
    ~OpenLendings();

private:
    Ui::OpenLendings *ui;
    Dataset *mDataset;
    LendBooksModel* mModel;
    void styleWindow();
    void setupTableView();

signals:
    void closewidget();

private slots:
    void comboBox_currentIndex_changed(int index);
    void customMenuRequested(QPoint point);
    void rowDoubleClicked(QModelIndex index);
    void singlebooking();
};

#endif // OPENLENDINGS_H
