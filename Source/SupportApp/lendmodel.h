#ifndef LENDMODEL_H
#define LENDMODEL_H

#include <QAbstractTableModel>
#include "../db/studenttable.h"
#include "../db/booktable.h"
#include "../db/lendingtable.h"
#include <QMap>
#include <QFontMetrics>
#include <QStyledItemDelegate>
#include <QHeaderView>
#include <QList>
#include <QRect>
#include <QVector>


class LendModel : public QAbstractTableModel
{
public:
    LendModel(StudentTable::StudentRow* srow, StudentTable* stable, BookTable* btable, LendingTable* ltable,
              QFontMetrics font, QObject * parent = 0);
    ~LendModel();
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void reset();

private:
    BookTable* mBookTable;
    LendingTable* mLendingTable;
    StudentTable* mStudentTable;
    struct LendInfo{QList<int> id;};
    struct Student {QString comment; QString surname; QString prename; QString dateofbirth; QList<ClassTable::ClassRow> classRows; QString address; QMap<QString, LendInfo> lendings;};
    Student StudentWithLendings;
    struct Book {QString isbn; QString title;};
    QVector<Book> mBooks;
    QString mClass;
    int mGrade;
    QString mSubject;
    QFontMetrics mFont;
    QWidget* mParent;
    bool lendNewBook(Book book, Student &student);
    bool giveBackBook(Book book, Student &student, QString damage);
    StudentTable::StudentRow mStudent;

};

class ItemDelegate : public QStyledItemDelegate
{
public:
    ItemDelegate(QObject *parent = 0);
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
};


#endif // LENDMODEL_H
