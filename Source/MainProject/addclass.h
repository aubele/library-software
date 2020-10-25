#ifndef ADDCLASS_H
#define ADDCLASS_H

#include <QDialog>
#include <dataset.h>


namespace Ui {
class addclass;
}

class addclass : public QDialog
{
    Q_OBJECT

public:
    explicit addclass(QWidget *parent = 0);
    ~addclass();
    ClassTable::ClassRow getNewClass();

private:
    Ui::addclass *ui;
    QString className;
    QString subject;
    ClassTable::ClassRow newClass;
    void styleWindow();
    void setupComboBoxSubject();

private slots:
    void add();
};

#endif // ADDCLASS_H
