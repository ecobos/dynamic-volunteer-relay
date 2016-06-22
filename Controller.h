#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QSslSocket>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonValue>

#include "RelayServer.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);
    void goOnline();

signals:
    void stop();
    void getSP();

public slots:
    void onStaticProxyAttained(QNetworkReply*);
    void saveConfiguration(QNetworkReply*);
    void stopConnections();
    void getAvailableStaticProxies();


private:
    const QString mBaseURL = "https://api.censorbuster.com";

    RelayServer *mRelay;
    QJsonDocument mResponse;
    QString mToken;
    QString mUUID;
    QString mPassword;

    void startRelayServer();
    void login();
    void loadConfiguration();
    void getConfiguration();
    void saveX509(QJsonValue);
};

#endif // CONTROLLER_H
