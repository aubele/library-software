#ifndef SELECTCLASS_H
#define SELECTCLASS_H

#include <QDialog>
#include <classtable.h>
#include "classmodel.h"
#include "dataset.h"

namespace Ui {
class SelectClass;
}

class SelectClass : public QDialog
{
    Q_OBJECT

public:
    explicit SelectClass(Dataset* set, QList<ClassTable::ClassRow> classes, QWidget *parent = 0);
    ~SelectClass();
    QList<ClassTable::ClassRow> getSelectedClasses();

private:
    Dataset* mDataset;
    Ui::SelectClass *ui;
    void mergeClasses();
    ClassTable::ClassRow mAddedClass;
    ClassModel* mModel;
    void styleWindow();

signals:
    void close();

private slots:
    void setupTableView();
    void newClass();
};

#endif // SELECTCLASS_H
