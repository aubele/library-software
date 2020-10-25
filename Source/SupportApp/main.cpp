#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QDebug>


//============================
// Implementations of Main
//============================
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    QFile dbfile("./database.db");
//    if (!dbfile.exists())
//    {
//        QFile databasefile("assets:/database.db");
//        databasefile.copy("./database.db");
//        if (!QFile::setPermissions("./database.db", QFile::WriteOwner | QFile::ReadOwner))
//            return -1;
//        qDebug() << "Keine Datenbank vorhanden.";
//    }
    QFile configFile("./App.config");
    if (!configFile.exists())
    {
        QFile configurationFile ("assets:/App.config");
        configurationFile.copy("./App.config");
        if (!QFile::setPermissions("./App.config", QFile::WriteOwner | QFile::ReadOwner))
            return -1;
    }
    MainWindow w;
    w.show();

    return a.exec();
}
