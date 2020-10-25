#ifndef BOOKMODEL_H
#define BOOKMODEL_H

#include <QAbstractTableModel>
#include <booktable.h>
#include <lendingtable.h>
#include <QVector>

class BookModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit BookModel(BookTable *table, LendingTable *ltable, QObject *parent = nullptr);
    ~BookModel();

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void reset();
    void search(QString searchstring);

    BookTable::BookRow* getRow(QModelIndex row);
    void updateRow(int row);

    void addRow(BookTable::BookRow * newRow);

private:
    BookTable *mBookTable;
    LendingTable *mLendTable;
    QVector<BookTable::BookRow> mBooks;
};

#endif // BOOKMODEL_H
