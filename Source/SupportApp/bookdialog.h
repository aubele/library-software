#ifndef BOOKDIALOG_H
#define BOOKDIALOG_H

#include <QDialog>
#include <dataset.h>
#include "booklendmodel.h"


namespace Ui {
class BookDialog;
}

class BookDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BookDialog(Dataset* set, QString classname, QWidget *parent = 0);
    ~BookDialog();

private:
    Ui::BookDialog *ui;
    Dataset* mDataset;
    QString classname;
    QList<StudentTable::StudentRow> students;
    BookTable* mBookTable;
    LendingTable* mLendTable;
    StudentTable* mStudentTable;
    ClassTable* mClassTable;
    BookLendModel* mModel;

private slots:
    void setupTableView();
    void goBack();
};

#endif // BOOKDIALOG_H
