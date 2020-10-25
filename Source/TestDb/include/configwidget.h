#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include <QWidget>
#include <QDialog>
#include "config.h"
#include <QVBoxLayout>
#include "baseqt_global.h"

class BASEQTSHARED_EXPORT ConfigDialog
{
public:
    virtual void setModified(bool modified) = 0;
};

class BASEQTSHARED_EXPORT ConfigWidget : public QWidget, public ConfigObserver
{
    Q_OBJECT
public:
    explicit ConfigWidget(Config* cfg, ConfigDialog *parentDialog, QWidget *parent = nullptr);
    ~ConfigWidget();
    void cfgblockchanged(QString blockname, Action action) noexcept;
    void cfgkeyofblockchanged(QString blockname, QString keyname, Action action) noexcept;

private:
    Config* mCfg;
    ConfigDialog* mParentDialog;
    void addElements(Config* cfg, QVBoxLayout* layout);

private slots:
    void hideClicked();
};

#endif // CONFIGWIDGET_H
