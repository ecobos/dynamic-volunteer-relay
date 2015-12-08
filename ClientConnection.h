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

#include "sslserver.h"
#include "StaticProxyConnection.h"

#include <QDebug>  // For Debugging purposes


class ClientConnection : public QObject
{
    Q_OBJECT
public:
    explicit ClientConnection(QObject *parent = 0);
    void setSP(const QString & host, quint16 port);


signals:
    void getSP();
    void stop();

public slots:
    void acceptNewConnection();
    void stopListening();
    void clientToServerWrite();
    void serverToClientWrite();


private:
    void connectToSPorFail();
    SslServer *mSslServer;
    QSslSocket *mClientSocket; 
    StaticProxyConnection *mSPConnection;
    QString mSP_Host;
    quint16 mSP_Port;
};

#endif // CLIENTCONNECTION_H
