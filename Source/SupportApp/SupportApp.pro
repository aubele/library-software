#-------------------------------------------------
#
# Project created by QtCreator 2018-10-01T18:18:44
#
#-------------------------------------------------

QT       += core gui sql
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SupportApp
TEMPLATE = app


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    ../baseqt/lizenzplugin.cpp \
    ../baseqt/aboutdialog.cpp \
    ../baseqt/config.cpp \
    ../baseqt/baseexception.cpp \
    ../baseqt/configwidget.cpp \
    ../baseqt/configwidgetelement.cpp \
    ../baseqt/configcache.cpp \
    ../baseqt/variousvalue.cpp \
    ../baseqt/cfgblock.cpp \
    ../baseqt/cfgkey.cpp \
    ../baseqt/settingsdialog.cpp \
    ../db/lendingtable.cpp \
    ../db/classtable.cpp \
    ../db/givebacktable.cpp \
    ../db/booktable.cpp \
    ../db/studenttable.cpp \
    ../db/studentclasstable.cpp \
    ../db/usertable.cpp \
    ../db/databaserestoration.cpp \
    ../db/logindialog.cpp \
    ../db/databasebackup.cpp \
    ../db/abstractdatabase.cpp \
    dataset.cpp \
    selectclassdialog.cpp \
    classdialog.cpp \
    studentmodel.cpp \
    pupilbookdialog.cpp \
    lendmodel.cpp \
    bookdialog.cpp \
    booklendmodel.cpp \
    damagedialog.cpp


HEADERS += \
        mainwindow.h \
    ../baseqt/lizenzplugin.h \
    ../baseqt/baseexception.h \
    ../baseqt/aboutdialog.h \
    ../baseqt/configwidget.h \
    ../baseqt/configwidgetelement.h \
    ../baseqt/config.h \
    ../baseqt/configcache.h \
    ../baseqt/variousvalue.h \
    ../baseqt/baseqt_global.h \
    ../baseqt/cfgblock.h \
    ../baseqt/cfgkey.h \
    ../baseqt/settingsdialog.h \
    ../db/classtable.h \
    ../db/booktable.h \
    ../db/givebacktable.h \
    ../db/studenttable.h \
    ../db/studentclasstable.h \
    ../db/usertable.h \
    ../db/databaserestoration.h \
    ../db/lendingtable.h \
    ../db/logindialog.h \
    ../db/include/aboutdialog.h \
    ../db/databasebackup.h \
    ../db/db_global.h \
    ../db/abstractdatabase.h \
    ../db/include/configwidget.h \
    ../db/include/configwidgetelement.h \
    ../db/include/config.h \
    ../db/include/configcache.h \
    ../db/include/license.h \
    ../db/include/variousvalue.h \
    ../db/include/baseqt_global.h \
    ../db/include/cfgblock.h \
    ../db/include/cfgkey.h \
    dataset.h \
    abstractdatabase.h \
    selectclassdialog.h \
    classdialog.h \
    studentmodel.h \
    pupilbookdialog.h \
    lendmodel.h \
    bookdialog.h \
    booklendmodel.h \
    damagedialog.h

FORMS += \
        mainwindow.ui \
    ../db/databaserestoration.ui \
    ../db/excelimportdialog.ui \
    ../db/logindialog.ui \
    ../db/databasebackup.ui \
    ../baseqt/aboutdialog.ui \
    ../baseqt/settingsdialog.ui \
    selectclassdialog.ui \
    classdialog.ui \
    pupilbookdialog.ui \
    bookdialog.ui \
    damagedialog.ui

CONFIG += mobility
android  {
images_deploy.files=$$PWD/android/database.db
images_deploy.files+=$$PWD/android/App.config
images_deploy.path=/assets
INSTALLS += images_deploy
}

RESOURCES += \
    ressources.qrc
