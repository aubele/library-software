#ifndef STUDENTMODEL_H
#define STUDENTMODEL_H

#include <QAbstractTableModel>
#include <QStyledItemDelegate>

#include "../db/studenttable.h"
#include "../db/lendingtable.h"
#include <QVector>

class StudentModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    StudentModel(QList<StudentTable::StudentRow> students, LendingTable* ltable, QObject* parent = 0);
    ~StudentModel();

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void selectClass(QString className);
    void search(QString searchstring);
    void reset();
    StudentTable::StudentRow* getStudent(QModelIndex index);
    StudentTable::StudentRow* getStudent(int row);
    void updateRow(int row);

private:
    LendingTable* mLendigTable;
    QList<StudentTable::StudentRow> mStudents;
    QString mClass;
};

#endif // STUDENTMODEL_H
