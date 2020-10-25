#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "mylib.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    MyLib lib;
    lib.test();
}

MainWindow::~MainWindow()
{
    delete ui;
}
