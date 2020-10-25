#ifndef LIZENZPLUGIN_H
#define LIZENZPLUGIN_H

#include <QWidget>
#include <QDialog>
#include "baseqt_global.h"
#include <QPushButton>
#include <QWidget>
#include <QList>
#include <QVBoxLayout>

class BASEQTSHARED_EXPORT LicenceItem : public QObject
{
    Q_OBJECT

public:
    LicenceItem();
    virtual ~LicenceItem();
    virtual QPushButton* getButton(QWidget* parent);
    virtual QPushButton* getButton();
    virtual QWidget* getWidget(QWidget* parent);
    virtual QWidget* getWidget();
    virtual void removeWidget();

private:
    QPushButton* m_button;
    QWidget* m_widget;

signals:
    void new_item_selected(LicenceItem *item);

private slots:
    void is_toggled(bool checked);
};

class BASEQTSHARED_EXPORT Licenceplugin : public QObject
{
    Q_OBJECT

private:
    Licenceplugin();
    ~Licenceplugin();
    QList<LicenceItem*> m_list;
    static Licenceplugin* m_instance;
    QDialog *m_dialog;
    QVBoxLayout *m_grid;
    LicenceItem *m_last;

public:
    static void CreatePlugin();
    static void DeletePlugin();
    static void Add3rdPartyLicence(LicenceItem *item);
    static Licenceplugin* Instance();
    static void ShowLicences(QWidget *parent);

private slots:
    void closeEvent(QCloseEvent* event);
    void item_selected(LicenceItem* selected);
};

#endif // LIZENZPLUGIN_H
