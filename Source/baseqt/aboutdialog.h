#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QString>
#include "baseqt_global.h"

namespace Ui {
class AboutDialog;
}

class BASEQTSHARED_EXPORT AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QString programName, QString programVersion, QWidget *parent = 0);
    ~AboutDialog();

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
