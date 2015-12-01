#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QSslSocket>
#include <QNetworkReply>

#include "ClientConnection.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);

signals:
    void stop();
    void getSP();

public slots:
    void onConfigFileAttained(QNetworkReply*);
    void stopConnections();
    void getAvailableSPfromCC();

private:
    ClientConnection *mClient;
    void startClientServer();
};

#endif // CONTROLLER_H
