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
    QByteArray readBytesAvailable();
    void write(const QByteArray &);
    qint64 bytesAvailable();
    bool isOpen();
    void disconnect();

signals:
    void doRead();

public slots:
    void socketStateChanged(QAbstractSocket::SocketState);
    void socketError(QAbstractSocket::SocketError);
    void stop();
    void disconnected();

private:
    QTcpSocket* mSocket;
    QString mHost;
    quint16 mPort;
    QAbstractSocket::SocketState mSocketState;
    qint16 mTotalBytesWritten;
};

#endif // STATICPROXYCONNECTION_H
