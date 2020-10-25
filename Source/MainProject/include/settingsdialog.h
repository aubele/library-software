#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include "config.h"
#include "configwidget.h"
#include "baseqt_global.h"

namespace Ui {
class SettingsDialog;
}



class BASEQTSHARED_EXPORT SettingsDialog : public QDialog, public ConfigDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(Config *cfg, QWidget *parent = 0);
    ~SettingsDialog();
    void setModified(bool modified);

private:
    Ui::SettingsDialog *ui;
    bool mIsModified;
    void closeEvent(QCloseEvent *event);
    Config* mCfg;

private slots:
    void save();
};

#endif // SETTINGSDIALOG_H
