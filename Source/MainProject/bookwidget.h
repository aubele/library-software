#ifndef BOOKWIDGET_H
#define BOOKWIDGET_H

#include <QWidget>
#include "dataset.h"
#include <QTableView>
#include "bookmodel.h"

namespace Ui {
class BookWidget;
}

class BookWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BookWidget(Dataset *set, QWidget *parent = 0);
    ~BookWidget();

private:
    Ui::BookWidget *ui;
    Dataset *mDataset;
    BookModel *mModel;
    void setupTableView();
    void styleWindow();

private slots:
    void search();
    void customMenuRequested(QPoint pos);
    void openBookDialog();
    void deleteBook();
    void buttonAddBook_clicked();
    void rowDoubleClicked(QModelIndex index);

signals:
    void closewidget();
};

#endif // BOOKWIDGET_H
