#ifndef CLASSDIALOG_H
#define CLASSDIALOG_H

#include <QDialog>
#include "dataset.h"
#include "studentmodel.h"


namespace Ui {
class ClassDialog;
}

class ClassDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClassDialog(Dataset* set, QString classname, QWidget *parent = 0);
    ~ClassDialog();

private:
    Dataset* mDataset;
    Ui::ClassDialog *ui;
    QString classname;
    QList <StudentTable::StudentRow> allstudents;
    StudentModel* mModel;
    void setupTableView();

private slots:
    void openStudentBooks(QModelIndex index);
    void openBookDialog();
};

#endif // CLASSDIALOG_H
