/*
 * Edgar A. Cobos
 * CSULB Fall 2015
 * CECS 476 - Computer Security
 */

#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <QObject>
#include <QSslSocket>

#include "sslserver.h"
#include "staticproxyconnection.h"

#include <QDebug>  // For Debugging purposes


class ClientConnection : public QObject
{
    Q_OBJECT
public:
    explicit ClientConnection(QObject *parent = 0);

//signals:

public slots:
    void acceptNewConnection();
    void stopListening();

private:
    SslServer *mSslServer;
    QSslSocket *mSocket;

    StaticProxyConnection *mStaticProxy;
    QSslSocket *mClientSocket;
    QSslSocket *mStaticProxySocket;
};

#endif // CLIENTCONNECTION_H
