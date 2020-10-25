#ifndef COURSESUBJECTCORRECTIONDIALOG_H
#define COURSESUBJECTCORRECTIONDIALOG_H

#include <QDialog>
#include <QAbstractTableModel>
#include <classtable.h>

namespace Ui {
class CourseSubjectCorrectionDialog;
}

class EditableClassModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    EditableClassModel(ClassTable* ctable, QObject * parent = 0);
    ~EditableClassModel();
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void searchClass(QString classname);
    void reset();

private:
    ClassTable* mClassTable;
    QList<ClassTable::ClassRow> mRows;
};

class CourseSubjectCorrectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CourseSubjectCorrectionDialog(ClassTable *table, QWidget *parent = 0);
    ~CourseSubjectCorrectionDialog();

private:
    Ui::CourseSubjectCorrectionDialog *ui;
    EditableClassModel *mModel;
};

#endif // COURSESUBJECTCORRECTIONDIALOG_H
