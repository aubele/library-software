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
    explicit DamageDialog(QString booktitle, QWidget *parent = 0);
    ~DamageDialog();
    QString getDamage();

private:
    Ui::DamageDialog *ui;

private slots:
    void checkbox_clicked(int state);
};

#endif // DAMAGEDIALOG_H
