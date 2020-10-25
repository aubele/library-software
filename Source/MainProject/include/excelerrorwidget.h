#ifndef EXCELERRORWIDGET_H
#define EXCELERRORWIDGET_H

#include <QWidget>
#include "studenttable.h"
#include <QList>
#include <QPlainTextEdit>


struct WinQdEntry
{
    QString name;
    QList<QString> classes;
};
struct ExcelImportError
{
    QString message;
    QList<StudentTable::StudentRow> asvEntries;
    WinQdEntry winQDEntry;
};


class ExcelImportErrors
{
public:
    ExcelImportErrors();
    void addError(QString message, QList<StudentTable::StudentRow> asvEntries = QList<StudentTable::StudentRow>(), WinQdEntry winQDName = WinQdEntry());
    bool save();
    bool load();
    QList<ExcelImportError> mList;
};


class ExcelErrorTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit ExcelErrorTextEdit(ExcelImportErrors errors, QWidget *parent = 0);
    ~ExcelErrorTextEdit();
};

#endif // EXCELERRORWIDGET_H
