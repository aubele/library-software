#ifndef BOOKMASK_H
#define BOOKMASK_H

#include <QDialog>
#include <booktable.h>

namespace Ui {
class BookMask;
}

class BookMask : public QDialog
{
    Q_OBJECT

public:
    explicit BookMask(BookTable *table, QWidget *parent = 0);
    ~BookMask();
    BookTable::BookRow* getInsertedBookRow();
    void setBookRow(BookTable::BookRow* row);

private:
    Ui::BookMask *ui;
    bool mManipulateRow;
    BookTable::BookRow* mRow;
    BookTable *mTable;
    void checkCheckboxes(QList<int> grades);
    QList<int> getSelectedGrade();
    void setupComboBoxSubject();
    void styleWindow();

private slots:
    void button_save_clicked();
};

#endif // BOOKMASK_H
