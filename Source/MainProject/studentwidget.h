#ifndef STUDENTWIDGET_H
#define STUDENTWIDGET_H

#include "dataset.h"
#include "studentmodel.h"
#include <usertable.h>

#include <QWidget>
#include <QTableView>

// Forward declare, else build error
class StudentModel;

namespace Ui {
class StudentWidget;
}

class StudentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StudentWidget(Dataset* set, UserPermissions permission, QString* lastselectedClass, QWidget *parent = 0);
    ~StudentWidget();
private:
    void setupClassBox();
    void setupTableView();

    Dataset* mDataset;
    Ui::StudentWidget *ui;
    StudentModel* mModel;
    int mRowToDelete;
    UserPermissions mPermissions;
    QString *mLastClass;
    void styleWindow();

signals:
    void closewidget();
private slots:
    void comboBox_class_currentIndex_changed(int index);
    void customMenuRequested(QPoint pos);
    void openStudentDialog();
    void editStudent();
    void deleteStudent();
    void singlebooking();
    void search();
    void rowDoubleClicked(QModelIndex index);
};

#endif // STUDENTWIDGET_H
