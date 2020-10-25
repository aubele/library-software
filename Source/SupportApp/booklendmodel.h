#ifndef BOOKLENDMODEL_H
#define BOOKLENDMODEL_H

#include <QAbstractTableModel>
#include <QStyledItemDelegate>
#include "../db/studentclasstable.h"
#include "../db/booktable.h"
#include "../db/lendingtable.h"
#include "../db/studenttable.h"



class BookLendModel : public QAbstractTableModel
{
public:
    BookLendModel(QString classname, QList<StudentTable::StudentRow> srows, BookTable* btable, LendingTable* ltable,
                  StudentTable * stable, ClassTable* ctable, QFontMetrics font, QObject * parent = 0);
    ~BookLendModel();
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void reset();

private:
    QString classname;
    QList<StudentTable::StudentRow> msrows;
    BookTable* mBookTable;
    LendingTable* mLendingTable;
    StudentTable* mStudentTable;
    ClassTable* mClassTable;
    struct LendInfo{QList<int> id;};
    struct Student {QString comment; QString surname; QString prename; QString dateofbirth; QList<ClassTable::ClassRow> classRows; QString address; QMap<QString, LendInfo> lendings;};
    QList<Student> StudentsWithLendings;
    struct Book {QString isbn; QString title;};
    QVector<Book> mBooks;
    int mGrade;
    QString mSubject;
    QFontMetrics mFont;
    QWidget* mParent;
    bool lendNewBooks(Book book);
    bool giveBackBooks(Book book);
    StudentTable::StudentRow mStudent;

};
class BookItemDelegate : public QStyledItemDelegate
{
public:
    BookItemDelegate(QObject *parent = 0);
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
};


#endif // BOOKLENDMODEL_H
