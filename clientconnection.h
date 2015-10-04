/*
 * Edgar A. Cobos
 * CSULB Fall 2015
 * CECS 476 - Computer Security
 */

#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <QObject>
#include "sslserver.h"

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
};

#endif // CLIENTCONNECTION_H
