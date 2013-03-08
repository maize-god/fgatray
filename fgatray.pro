#-------------------------------------------------
#
# Project created by QtCreator 2013-03-08T00:31:32
#
#-------------------------------------------------

QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fgatray
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    fgadvice.cpp

HEADERS  += mainwindow.h \
    defaults.h \
    fgadvice.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc
