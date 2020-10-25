#-------------------------------------------------
#
# Project created by QtCreator 2018-04-17T16:18:09
#
#-------------------------------------------------

QT       += sql
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = db
TEMPLATE = lib

DEFINES += DB_LIBRARY

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
    usertable.cpp \
    abstractdatabase.cpp \
    studenttable.cpp \
    booktable.cpp \
    classtable.cpp \
    lendingtable.cpp \
    givebacktable.cpp \
    databasebackup.cpp \
    excelimportdialog.cpp \
    logindialog.cpp \
    excelerrorwidget.cpp \
    studentclasstable.cpp \
    databaserestoration.cpp

HEADERS += \
        db_global.h \ 
    usertable.h \
    abstractdatabase.h \
    studenttable.h \
    booktable.h \
    classtable.h \
    lendingtable.h \
    givebacktable.h \
    databasebackup.h \
    excelimportdialog.h \
    logindialog.h \
    excelerrorwidget.h \
    studentclasstable.h \
    databaserestoration.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    databasebackup.ui \
    excelimportdialog.ui \
    logindialog.ui \
    databaserestoration.ui

unix|win32: LIBS += -L$$PWD/lib/ -lExcelReader

unix|win32: LIBS += -L$$PWD/lib/ -lbaseqt

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
