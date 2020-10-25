#ifndef CLASSMODEL_H
#define CLASSMODEL_H
#include <QAbstractTableModel>
#include "classtable.h"
#include "studentclasstable.h"
#include "dataset.h"

#include <QVector>
#include <QCheckBox>

class ClassModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    ClassModel(Dataset* set, QObject* parent = 0);
    ~ClassModel();
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    void initialize();
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QList<ClassTable::ClassRow> getSelectedClasses();
    void addClass(ClassTable::ClassRow newClass);
    void selectClasses(QList<ClassTable::ClassRow> toSelectClasses);
    void reset();
    void fill();


private:
    Dataset* mDataset;
    ClassTable* mClassTable;
    StudentClassTable* mStudentClassTable;
    struct Class {bool selected; QString className; int grade; QString subject;};
    QVector<Class> mClasses;
    QList<Class> newcrows;
    Class mClass;
};

#endif // CLASSMODEL_H




