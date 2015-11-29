#ifndef STATICPROXYCONNECTION_H
#define STATICPROXYCONNECTION_H

#include <QObject>
#include <QSslSocket>

class StaticProxyConnection : public QObject
{
    Q_OBJECT
public:
    explicit StaticProxyConnection(QObject *parent = 0, const QString &host ="localhost", quint16 port = 0);
    void startConnection();
    void write(const QString);
    void disconnect();
    bool ping();

signals:
    //void disconnected();

public slots:
    void connected();
    void bytesWritten(qint64 bytes);
    QByteArray read();
    void socketStateChanged(QAbstractSocket::SocketState);
    void socketEncrypted();
    void socketError(QAbstractSocket::SocketError);
    void sslErrors(const QList<QSslError> &);
    void stop();

private:
    QSslSocket* mSocket;
    QString mHost;
    quint16 mPort;
    QAbstractSocket::SocketState mSocketState;
};

#endif // STATICPROXYCONNECTION_H
