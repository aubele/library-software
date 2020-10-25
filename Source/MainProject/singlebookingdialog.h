#ifndef SINGLEBOOKINGDIALOG_H
#define SINGLEBOOKINGDIALOG_H

#include <QDialog>
#include <studenttable.h>
#include <booktable.h>
#include <lendingtable.h>
#include <QAbstractTableModel>

namespace Ui {
class SingleBookingDialog;
}

class ShortBookModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum option {BooksForClass, AllBooks, LendBooks};
    explicit ShortBookModel(StudentTable::StudentRow *row, StudentTable* stable, BookTable* btable, LendingTable* ltable, QWidget *parent = nullptr);
    ~ShortBookModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void select(option option);
    void setHolidayLending(bool value);

private:
    struct LendInfo{QList<int> id;}; // TODO: not id but list of ids therefore amount is not required
    struct Student {QString comment; QString surname; QString forename; QString dateofbirth; QMap<QString, LendInfo> lendings;};
    QList<BookTable::BookRow> mBooks;
    StudentTable *mSTable;
    BookTable *mBTable;
    LendingTable *mLTable;
    bool mHolidayLending;
    Student mStudent;
    option mOption;
    StudentTable::StudentRow *mStudentRow;

    bool lendNewBook(BookTable::BookRow book, Student &student);
    bool giveBackBook(BookTable::BookRow book, Student &student, QString damage);
};


class SingleBookingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SingleBookingDialog(StudentTable::StudentRow *row, StudentTable* stable, BookTable* btable, LendingTable* ltable, QWidget *parent = 0);
    ~SingleBookingDialog();

private:
    Ui::SingleBookingDialog *ui;
    ShortBookModel *mModel;
    void styleWindow();

private slots:
    void comboBox_bookingoptions_currentIndex_changed(int index);
    void radioButton_changed(bool value);
};

#endif // SINGLEBOOKINGDIALOG_H
