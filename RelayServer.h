/*
 * Edgar A. Cobos
 * CSULB Fall 2015
 * CECS 476 - Computer Security
 */

#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <QObject>
#include <QString>
#include <QSslSocket>
#include <QDataStream>

#include "SslServer.h"
#include "StaticProxyConnection.h"

#include <QDebug>  // For Debugging purposes


class RelayServer : public QObject
{
    Q_OBJECT
public:
    explicit RelayServer(QObject *parent = 0);
    void setStaticProxy(const QString & host, quint16 port);


signals:
    void getStaticProxy();
    void stop();

public slots:
    void acceptNewConnection();
    void stopListening();
    void clientToServerWrite();
    void serverToClientWrite();
    void handshakeSuccessful();
    void onSslErrors(const QList <QSslError> &);
    void onPeerVerifyError(const QSslError &);


private:
    void connectToSPorFail();
    SslServer *mSslServer;
    QSslSocket *mClientSocket; 
    QDataStream mClientDataStream;
    StaticProxyConnection *mStaticProxy;
    QString mSP_Host;
    quint16 mSP_Port;
};

#endif // CLIENTCONNECTION_H
