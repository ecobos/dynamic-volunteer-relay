#-------------------------------------------------
#
# Project created by QtCreator 2015-09-30T20:56:09
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SecondAttempt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    clientconnection.cpp \
    sslserver.cpp

HEADERS  += mainwindow.h \
    clientconnection.h \
    sslserver.h

FORMS    += mainwindow.ui

RESOURCES = certificate.qrc
