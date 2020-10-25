#-------------------------------------------------
#
# Project created by QtCreator 2018-04-06T22:27:14
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MainProject
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
    dataset.cpp \
    bookingwidget.cpp \
    studentlendmodel.cpp \
    studentmodel.cpp \
    studentwidget.cpp \
    damagedialog.cpp \
    bookwidget.cpp \
    bookmodel.cpp \
    bookmask.cpp \
    studentmask.cpp \
    addclass.cpp \
    selectclass.cpp \
    classmodel.cpp \
    delegates.cpp \
    singlebookingdialog.cpp \
    adduserdialog.cpp \
    usermanagementdialog.cpp \
    OpenLendings.cpp \
    DamagesWidget.cpp \
    coursesubjectcorrectiondialog.cpp \
    comboboxmodel.cpp \
    appwidget.cpp

HEADERS += \
        mainwindow.h \
    dataset.h \
    bookingwidget.h \
    studentlendmodel.h \
    studentmodel.h \
    studentwidget.h \
    damagedialog.h \
    bookwidget.h \
    bookmodel.h \
    bookmask.h \
    studentmask.h \
    addclass.h \
    selectclass.h \
    classmodel.h \
    delegates.h \
    singlebookingdialog.h \
    adduserdialog.h \
    usermanagementdialog.h \
    OpenLendings.h \
    DamagesWidget.h \
    coursesubjectcorrectiondialog.h \
    comboboxmodel.h \
    appwidget.h

FORMS += \
        mainwindow.ui \
    bookingwidget.ui \
    studentwidget.ui \
    damagedialog.ui \
    bookwidget.ui \
    bookmask.ui \
    studentmask.ui \
    addclass.ui \
    selectclass.ui \
    singlebookingdialog.ui \
    adduserdialog.ui \
    usermanagementdialog.ui \
    OpenLendings.ui \
    DamagesWidget.ui \
    coursesubjectcorrectiondialog.ui \
    appwidget.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lbaseqt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lbaseqt
else:unix: LIBS += -L$$PWD/lib/ -lbaseqt

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

unix|win32: LIBS += -L$$PWD/lib/ -ldb

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

RESOURCES += \
    ressources.qrc

win32:CONFIG(release, debug|release): RC_FILE = MainProject.rc
else:win32:CONFIG(debug, debug|release): RC_FILE = MainProject.rc

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
