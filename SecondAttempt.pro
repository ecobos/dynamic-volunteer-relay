#-------------------------------------------------
#
# Project created by QtCreator 2015-09-30T20:56:09
#
#-------------------------------------------------

QT       += core gui network
#OPENSSL_LIBS = '-L/usr/local/Cellar/openssl/1.0.2d_1/lib -lssl -lcrypto' ./configure -openssl-linked
OPENSSL_LIBS = '/usr/local/Cellar/openssl/1.0.2d_1/lib/libcrypto.a /usr/local/Cellar/openssl/1.0.2d_1/lib/libssl.a -lssl -lcrypto' ./configure -v -openssl-linked
QT_LOGGING_RULES = qt.network.ssl.warning=false

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SecondAttempt
TEMPLATE = app


SOURCES += main.cpp\
    clientconnection.cpp \
    sslserver.cpp \
    staticproxyconnection.cpp \
    mainwindow.cpp \
    dynamicvolunteer.cpp

HEADERS  += mainwindow.h \
    clientconnection.h \
    sslserver.h \
    staticproxyconnection.h \
    dynamicvolunteer.h

FORMS    += mainwindow.ui

RESOURCES = certificate.qrc
