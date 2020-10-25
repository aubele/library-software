#ifndef BOOKINGWIDGET_H
#define BOOKINGWIDGET_H

#include <QWidget>
#include "dataset.h"
#include <QTableView>
#include "studentlendmodel.h"

namespace Ui {
class BookingWidget;
}

class BookingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BookingWidget(Dataset *set, QString* lastselectedClass, QWidget *parent = 0);
    ~BookingWidget();

private:
    Ui::BookingWidget *ui;
    Dataset *mDataset;
    QTableView *mView;
    QString *mLastClass;
    StudentLendModel *mModel;
    void styleWindow();

signals:
    void closewidget();

private slots:
    void comboBox_currentIndex_changed();
    StudentLendModel::BookOption get_current_comboBox_bookingoption();
    void search();
};

#endif // BOOKINGWIDGET_H
