#-------------------------------------------------
#
# Project created by QtCreator 2018-05-24T10:37:16
#
#-------------------------------------------------

QT       -= gui

TARGET = ExcelReader
TEMPLATE = lib

DEFINES += EXCELREADER_LIBRARY

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
        excelreader.cpp \
    freexl.cpp \
    localcharset.cpp

HEADERS += \
        excelreader.h \
        excelreader_global.h \ 
    freexl.h \
    freexl_internals.h \
    config.h \
    config-msvc.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
