#ifndef STATICPROXYCONNECTION_H
#define STATICPROXYCONNECTION_H

#include <QObject>
#include <QSslSocket>

class StaticProxyConnection : public QObject
{
    Q_OBJECT
public:
    explicit StaticProxyConnection(QObject *parent = 0);
    void connectTo(const QString &, quint16);
    bool ping();
    QByteArray read();
    void write(const QByteArray &);
    qint64 bytesAvailable();
    bool isOpen();
    void disconnect();

signals:
    //void disconnected();
    void readyRead();

public slots:
    void connected();
    void bytesWritten(qint64 bytes);
    void readReady();
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
