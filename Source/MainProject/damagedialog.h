#ifndef DAMAGEDIALOG_H
#define DAMAGEDIALOG_H

#include <QDialog>

namespace Ui {
class DamageDialog;
}

class DamageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DamageDialog(QString forename, QString surname, QString booktitle, QWidget *parent = 0);
    ~DamageDialog();
    QString getDamage();

private:
    Ui::DamageDialog *ui;
    void styleWindow();

private slots:
    void checkbox_changed(int state);
};

#endif // DAMAGEDIALOG_H
