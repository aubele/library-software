#ifndef CONFIGWIDGETELEMENT_H
#define CONFIGWIDGETELEMENT_H

#include <QWidget>
#include <QString>
#include "config.h"
#include <QLabel>
#include <QPushButton>
#include "baseqt_global.h"


class BASEQTSHARED_EXPORT ConfigWidgetElement : public QWidget
{
    Q_OBJECT
public:
    explicit ConfigWidgetElement(QString blockname, QString keyname, Config* cfg, QWidget *parent = nullptr);
    ~ConfigWidgetElement();

private slots:
    void checkboxChanged(int state);
    void dspinboxchanged(double newvalue);
    void spinboxchanged(int newvalue);
    void textchanged(QString newvalue);
    void colorbuttonclicked();
    void comboboxtextchanged(QString newvalue);

private:
    Config* mCfg;
    QLabel* mName;
    QWidget* mEdit;
    QLabel* mComment;
    QPushButton* mColorButton;
    QString mKeyname;
    QString mBlockname;
};

#endif // CONFIGWIDGETELEMENT_H
