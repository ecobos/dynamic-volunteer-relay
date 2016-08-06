#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QSslSocket>
#include <QNetworkReply>
#include <QJsonValue>
#include <QJsonDocument>

#include "RelayServer.h"
#include "CommandControlInterface.h"

struct Credentials{
    QString uuid;
    QString password;
};

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);
    void goOnline();
    void startUp();

signals:
    void stop();
    void getSP();

public slots:
    void onStaticProxyAttained(QNetworkReply*);

    void stopConnections();


private:

    RelayServer* mRelay;
    CommandControlInterface* mCommandControl;
    QJsonDocument mResponse;


    void startRelayServer(const QString &);
    void login();
    Credentials loadConfiguration();
    void getConfiguration();
    void saveX509(QJsonValue);
};

#endif // CONTROLLER_H
