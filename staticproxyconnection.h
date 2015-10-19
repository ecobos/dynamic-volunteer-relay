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

signals:
    //void disconnected();

public slots:
    void connected();
    void bytesWritten(qint64 bytes);
    void readyRead();
    void disconnected();
    void socketStateChanged(QAbstractSocket::SocketState);
    void socketEncrypted();
    void socketError(QAbstractSocket::SocketError);
    void sslErrors(const QList<QSslError> &);

private:
    QSslSocket* mSocket;
    QString mHost;
    quint16 mPort;
};

#endif // STATICPROXYCONNECTION_H
