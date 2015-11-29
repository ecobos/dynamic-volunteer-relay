#ifndef DYNAMICVOLUNTEER_H
#define DYNAMICVOLUNTEER_H

#include <QObject>
#include <QSslSocket>
#include <QNetworkReply>

#include "clientconnection.h"

class DynamicVolunteer : public QObject
{
    Q_OBJECT
public:
    explicit DynamicVolunteer(QObject *parent = 0);

signals:
    void stop();

public slots:
    void onConfigFileAttained(QNetworkReply*);
    void stopConnections();
    void getAvailableSPfromCC();

private:
    ClientConnection *mClient;
    void startClientServer();
};

#endif // DYNAMICVOLUNTEER_H
