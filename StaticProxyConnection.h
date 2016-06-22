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
    //void disconnected();
    void doRead();

public slots:
    void connected();
    void addToEncryptBytesWritten(qint64 bytes);
    void readyRead();
    void socketStateChanged(QAbstractSocket::SocketState);
    void socketEncrypted();
    void socketError(QAbstractSocket::SocketError);
    void sslErrors(const QList<QSslError> &);
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
