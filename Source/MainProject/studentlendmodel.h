#ifndef STUDENTMODEL_H
#define STUDENTMODEL_H

#include <QAbstractTableModel>
#include "studenttable.h"
#include "booktable.h"
#include "lendingtable.h"
#include <QMap>
#include <QFontMetrics>
#include <QStyledItemDelegate>
#include <QHeaderView>
#include <QList>
#include <QRect>
#include <QVector>

class StudentLendModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum BookOption {BooksForClass = 0, AllBooks = 1, LentBooks = 2};
    StudentLendModel(StudentTable* stable, BookTable* btable, LendingTable* ltable, QFontMetrics font, QObject * parent = 0);
    ~StudentLendModel();
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role);
    void selectClass(QString classname, int grade, QString subject, BookOption mOption);
    void selectBookOptions(BookOption option);
    void reset();
    void setDialogParent(QWidget *parent);
    void searchInBooks(QString searchString, BookOption mOption);
    StudentTable::StudentRow* getStudent(QModelIndex index);

private:
    StudentTable* mStudentTable;
    BookTable* mBookTable;
    LendingTable* mLendingTable;
    struct LendInfo{QList<int> id;};
    struct Student {QString comment; QString surname; QString forename; QString dateofbirth; QMap<QString, LendInfo> lendings;};
    QVector<Student> mStudents;
    struct Book {QString isbn; QString title;};
    QVector<Book> mBooks;
    QString mClass;
    int mGrade;
    QString mSubject;
    QFontMetrics mFont;
    QWidget* mParent;
    bool lendNewBook(Book book, Student &student);
    bool giveBackBook(Book book, Student &student, QString damage);
    bool areAllItemsOfSectionChecked(int section) const;

signals:
    void columnsmaychanged();
};

class ItemDelegate : public QStyledItemDelegate
{
public:
    ItemDelegate(QObject *parent = 0);
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
};

class CheckBoxHeader : public QHeaderView
{

public:
    CheckBoxHeader(Qt::Orientation orientation, QWidget* parent = 0);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

protected:
    void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const;
    void mousePressEvent(QMouseEvent* event);
};

class NameIconHeader : public QHeaderView
{
public:
    NameIconHeader(Qt::Orientation orientation, QWidget* parent = 0);

protected:
    void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const;
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent *e);
};


#endif // STUDENTMODEL_H
