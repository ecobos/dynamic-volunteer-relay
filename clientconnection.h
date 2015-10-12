/*
 * Edgar A. Cobos
 * CSULB Fall 2015
 * CECS 476 - Computer Security
 */

#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <QObject>
#include "sslserver.h"

#include <QNetworkReply>
#include <QDebug>  // For Debugging purposes

#include "staticproxyconnection.h"

class ClientConnection : public QObject
{
    Q_OBJECT
public:
    explicit ClientConnection(QObject *parent = 0);

//signals:

public slots:
    void acceptNewConnection();
    void stopListening();
    void onConfigFileAttained(QNetworkReply*);

private:
    SslServer *mSslServer;
};

#endif // CLIENTCONNECTION_H
