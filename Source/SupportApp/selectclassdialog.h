#ifndef SELECTCLASSDIALOG_H
#define SELECTCLASSDIALOG_H
#include <QDialog>
#include "dataset.h"
#include "../db/classtable.h"


namespace Ui {
class selectClassDialog;
}

class selectClassDialog : public QDialog
{
    Q_OBJECT

public:
    explicit selectClassDialog(QWidget *parent = 0);
    ~selectClassDialog();

private:
    Ui::selectClassDialog *ui;
    Dataset mDataSet;
    QStringList mClasses;
    void resetClasses();

private slots:
    void openPupils();
    void openSettings();

};

#endif // SELECTCLASSDIALOG_H
