#-------------------------------------------------
#
# Project created by QtCreator 2015-09-30T20:56:09
#
#-------------------------------------------------

QT       += core gui network
OPENSSL_LIBS ='-L/opt/ssl/lib -lssl -lcrypto' ./configure -openssl-linked
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SecondAttempt
TEMPLATE = app


SOURCES += main.cpp\
    clientconnection.cpp \
    sslserver.cpp \
    staticproxyconnection.cpp \
    mainwindow.cpp

HEADERS  += mainwindow.h \
    clientconnection.h \
    sslserver.h \
    staticproxyconnection.h

FORMS    += mainwindow.ui

RESOURCES = certificate.qrc
