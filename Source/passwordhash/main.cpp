#include "mainwindow.h"
#include <QApplication>
#include <QCryptographicHash>
#include <QDebug>

int main(int argc, char *argv[])
{

    QString passwd_welcome = QString(QCryptographicHash::hash("Welcome",QCryptographicHash::Md5).toHex());
    QString passwd_test = QString(QCryptographicHash::hash("Test",QCryptographicHash::Md5).toHex());

    qDebug()<<passwd_welcome;
    qDebug()<<passwd_test;
}
