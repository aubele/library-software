#include "mainwindow.h"
#include "QVBoxLayout"
#include "QFile"
#include "QLabel"
#include "QDir"
#include "QString"
#include "QUrl"
#include "QDesktopServices"
#include "ui_mainwindow.h"

#include "license.h"

//
//  EXAMPLE ON HOW TO USE THE LICENSE DIALOG
//
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    LicenseMenu menu;
    QMenu* ueber = menu.getLicenseMenu();
    enumerateMenu(ueber);
    ui->menuBar->addMenu(ueber);

}

void MainWindow::enumerateMenu(QMenu *menu)
{
    foreach (QAction *action, menu->actions()) {
        if (action->isSeparator()) {
        } else if (action->menu()) {
            enumerateMenu(action->menu());
        } else {
            if(action->text() == "LGPL")
            {
                connect(action, SIGNAL(triggered()), this, SLOT(openLgplFile()));
            }
            else if(action->text() == "GPL")
            {
                connect(action, SIGNAL(triggered()), this, SLOT(openGplFile()));
            }
        }
    }
}

void MainWindow::openGplFile()
{
    QString path = QDir::currentPath();
    path.append("/gpl_license.txt");
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void MainWindow::openLgplFile()
{
    QString path = QDir::currentPath();
    path.append("/lgpl_license.txt");
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

MainWindow::~MainWindow()
{
    delete ui;
}
