#ifndef STUDENTMODEL_H
#define STUDENTMODEL_H

#include <QAbstractTableModel>
#include <QStyledItemDelegate>

#include "studenttable.h"
#include "lendingtable.h"
#include <QVector>

class StudentModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    StudentModel(StudentTable* stable, LendingTable* ltable, QObject* parent = 0);
    ~StudentModel();

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void selectClass(QString className);
    void search(QString searchstringNameComment, QString searchstringAmount);
    void deleteStudent(int row);
    void reset();
    StudentTable::StudentRow* getStudent(QModelIndex index);
    StudentTable::StudentRow* getStudent(int row);
    void updateRow(int row);
    void addStudent(StudentTable::StudentRow* newRow);

private:
    StudentTable* mStudentTable;
    LendingTable* mLendigTable;
    QVector<StudentTable::StudentRow> mStudents;
    QString mClass;
};

class ItemDelegateStudent : public QStyledItemDelegate
{
public:
    ItemDelegateStudent(QObject *parent = 0);
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

#endif // STUDENTMODEL_H
