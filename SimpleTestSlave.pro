#-------------------------------------------------
#
# Project created by QtCreator 2018-01-08T15:38:31
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SimpleTestSlave
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    SlaveThread.cpp

HEADERS  += mainwindow.h \
    SlaveThread.h

FORMS    += mainwindow.ui
