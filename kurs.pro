#-------------------------------------------------
#
# Project created by QtCreator 2014-09-30T02:02:28
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kurs
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    samoplayer.cpp \
    bd.cpp

HEADERS  += mainwindow.h \
    samoplayer.h

FORMS    += mainwindow.ui

RESOURCES += \
    res1.qrc
LIBS += -lsqlite3
