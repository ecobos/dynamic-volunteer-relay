/*
 * Edgar A. Cobos
 * CSULB Fall 2015
 * CECS 476 - Computer Security
 *
 * Forked from https://github.com/GuiTeK/Qt-SslServer/
 */

#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QTcpServer>
#include <Qssl>
#include <QSslCertificate>
#include <QSslKey>
#include <QSslSocket>

class SslServer : public QTcpServer
{
public:
    SslServer(QObject * parent = 0);
    void setSslLocalCertificate(const QSslCertificate &certificate);
    void setSslPrivateKey(const QSslKey &key);
    //bool setSslPrivateKey(const QString &fileName, QSsl::KeyAlgorithm algorithm = QSsl::Rsa, QSsl::EncodingFormat format = QSsl::Pem, const QByteArray &passPhrase = QByteArray());
    void setSslProtocol(QSsl::SslProtocol protocol = QSsl::SecureProtocols);
    void setSslPeerVerifyMode(QSslSocket::PeerVerifyMode verifyMode = QSslSocket::VerifyPeer);
    //QTcpSocket* nextPendingConnection();

public slots:
    void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;
    void onSslErrors(const QList <QSslError> & errors);

private:

    QSslCertificate mLocalCertificate;
    QSslKey mPrivateKey;
    QSsl::SslProtocol mProtocol;
    QSslSocket::PeerVerifyMode mPeerMode;
};

#endif // SSLSERVER_H
