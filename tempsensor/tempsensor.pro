#-------------------------------------------------
#
# Project created by QtCreator 2016-06-19T13:42:13
#
#-------------------------------------------------

QT       += core gui serialport multimedia

CONFIG += c++11


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tempsensor
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    seriallistener.cpp

HEADERS  += dialog.h \
    seriallistener.h

FORMS    += dialog.ui

RESOURCES += \
    resources.qrc

