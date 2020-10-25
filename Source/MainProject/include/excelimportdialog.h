#ifndef EXCELIMPORTDIALOG_H
#define EXCELIMPORTDIALOG_H

#include "db_global.h"
#include "excelerrorwidget.h"
#include <excelreader.h>
#include "config.h"
#include "lendingtable.h"

#include <QThread>
#include <QDialog>


namespace Ui {
class ExcelImportDialog;
}

class ExcelImportThread : public QThread
{
    Q_OBJECT

public:
    ExcelImportThread(QString asvfile, QString q11file, QString q12file, StudentTable* studTable, LendingTable* lendTable);
    ExcelImportErrors getErrors();

protected:
    void run();

signals:
    void progress(int percentage, QString state);

private:
    bool importASV();
    void createTableEntry(QString values[], QMap<QString, int> mapping, bool containsOnlyName);
    void remove12thGraders();
    QString validateAddress(QString address);

    bool importWinQd(QString file, int version);
    QList< QPair<QString, StudentTable::StudentRow> > getStudentNamePatterns(int version);
    bool checkWinQdFormat(int col, FreeXL_CellValue cell);
    StudentTable::StudentRow getValidatedStudent(QString studentname, QList< QPair<QString, StudentTable::StudentRow> > correctNames, bool* error, QString file, ExcelReader reader, int row);
    WinQdEntry getWinQdEntry(ExcelReader reader, int row);
    ClassTable::ClassRow getClassRow(FreeXL_CellValue cell, int version);

    QString mAsvFile;
    QString mQ11File;
    QString mQ12File;
    ExcelImportErrors mErrors;
    StudentTable* mStudTable;
    LendingTable* mLendTable;
    Config* cfg;
};


class DBSHARED_EXPORT ExcelImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExcelImportDialog(StudentTable* table, LendingTable* lendTable, QWidget *parent = 0);
    ~ExcelImportDialog();

private:
    Ui::ExcelImportDialog *ui;
    bool mASVOK;
    bool mQ11OK;
    bool mQ12OK;
    ExcelImportThread* mThread;
    StudentTable* mStudTable;
    LendingTable* mLendTable;

private slots:
    void button_select_asv_file_clicked();
    void button_select_q11_file_clicked();
    void button_select_q12_file_clicked();
    void button_start_import_clicked();
    void button_show_error_clicked();
    void thread_progress(int percentage, QString state);
    void thread_finished();
};

#endif // EXCELIMPORTDIALOG_H
