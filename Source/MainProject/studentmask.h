#ifndef STUDENTMASK_H
#define STUDENTMASK_H

#include <QDialog>
#include "dataset.h"
#include <QVector>
#include <QAbstractTableModel>

class ShortBookModel;

namespace Ui {
class StudentMask;
}

class StudentMask : public QDialog
{
    Q_OBJECT

public:
    explicit StudentMask(Dataset* set, QWidget* parent = 0);
     ~StudentMask();
    StudentTable::StudentRow student;
    void selectStudent(StudentTable::StudentRow *row);
    StudentTable::StudentRow* getNewStudent();

private:
    QList<ClassTable::ClassRow> classes;
    Dataset* mDataset;
    Ui::StudentMask *ui;
    ShortBookModel* mModel;
    bool mModify;
    StudentTable::StudentRow *mReturnStudent;
    void styleWindow();

private slots:
    void editClasses();
    void applyData();
};

#endif // STUDENTMASK_H
