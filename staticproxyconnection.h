#ifndef STATICPROXYCONNECTION_H
#define STATICPROXYCONNECTION_H

#include <QObject>
#include <QTcpSocket>

class StaticProxyConnection : public QObject
{
    Q_OBJECT
public:
    explicit StaticProxyConnection(QObject *parent = 0, const QString &host ="localhost", quint16 port = 0);
    void startConnection();

signals:
    //void disconnected();

public slots:
    void connected();
    void bytesWritten(qint64 bytes);
    void readyRead();
    void disconnected();

private:
    QTcpSocket* mSocket;
    QString mHost;
    quint16 mPort;
};

#endif // STATICPROXYCONNECTION_H
