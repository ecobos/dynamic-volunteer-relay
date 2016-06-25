#ifndef COMMANDCONTROLINTERFACE_H
#define COMMANDCONTROLINTERFACE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>

enum Status
{
    ONLINE, OFFLINE
};

class CommandControlInterface : public QObject
{
    Q_OBJECT
public:
    explicit CommandControlInterface(QObject *parent = 0);
    ~CommandControlInterface();
    void setCredentials(const QString &, const QString &);
    void login();
    void status(Status);
    QString getAvailableStaticProxy();
    void shutdown(const QString &);
    QJsonObject* registerApplication();

    static const QString DOMAIN;
    static const QString LOGIN_ENDPOINT;
    static const QString DVP_ENDPOINT;
    static const QString SP_ENDPOINT;
    static const QString ONLINE_ENDPOINT;
    static const QString OFFLINE_ENDPOINT;

signals:

public slots:
    void requestError(QNetworkReply::NetworkError);

private:

    QNetworkAccessManager* mNetworkManager;

    QString mToken;
    QString mUUID;
    QString mPassword;
};

#endif // COMMANDCONTROLINTERFACE_H
