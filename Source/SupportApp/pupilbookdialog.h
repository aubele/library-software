#ifndef PUPILBOOKDIALOG_H
#define PUPILBOOKDIALOG_H

#include <QDialog>
#include "../db/studenttable.h"
#include "../db/booktable.h"
#include "../db/lendingtable.h"
#include "lendmodel.h"

namespace Ui {
class PupilBookDialog;
}

class PupilBookDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PupilBookDialog(int grade, StudentTable::StudentRow* row, StudentTable* stable, BookTable* btable, LendingTable* ltable, QWidget *parent = 0);
    ~PupilBookDialog();

private:
    Ui::PupilBookDialog *ui;
    void setupTableView();
    int mGrade;
    StudentTable::StudentRow* mRow;
    StudentTable* mStudentTable;
    BookTable* mBookTable;
    LendingTable* mLendingTable;
    QString studentName;
    QList<BookTable::BookRow> books;
    LendModel* mModel;
};

#endif // PUPILBOOKDIALOG_H
