#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dataset.h"
#include <usertable.h>
#include <QAction>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void rider_settings_triggered();
    void rider_about_triggered();
    void rider_import_triggered();
    void rider_changeuser_triggered();
    void rider_3rd_party_software_triggered();
    void rider_lend_and_giveback_triggered();
    void rider_books_triggered();
    void rider_students_triggered();
    void clearWidget();
    void rider_restoration_triggered();
    void rider_usermanagement_triggerd();
    void rider_open_lendings_triggered();
    void rider_damages_triggered();
    void rider_Kurs_Fach_Zuordnung_triggered();
    void rider_Unterstuetzungsapp_triggered();

protected:
    void closeEvent(QCloseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    Ui::MainWindow *ui;
    Dataset mDataSet;
    QString mUserName;
    void initUiWithPermissions (UserPermissions permissions);
    bool mIsInitialized;
    void window_loaded();
    QAction* mLastAction;
    UserPermissions mPermissions;
    QString mLastClass;
    void styleWindow();
};

#endif // MAINWINDOW_H
