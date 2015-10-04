/*
 * Edgar A. Cobos
 * CSULB Fall 2015
 * CECS 476 - Computer Security
 *
 * Forked from https://github.com/GuiTeK/Qt-SslServer/
 */

#include "sslserver.h"

SslServer::SslServer(QObject * parent) : QTcpServer(parent)
{ /*\ 0_0 \*/}

/**
 * Called when a new connection is available. The newConnection()
 *  signal is emitted when the connection is added to the pending
 *  connections queue
 *
 * @brief SslServer::incomingConnection
 * @param socketDescriptor
 *
 * Ref: http://doc.qt.io/qt-5/qtcpserver.html#incomingConnection
 */
void SslServer::incomingConnection(qintptr socketDescriptor)
{
    QSslSocket *mSslSocket = new QSslSocket(this);
    mSslSocket->setSocketDescriptor(socketDescriptor);
    mSslSocket->setProtocol(mProtocol);
    mSslSocket->setLocalCertificate(mLocalCertificate);
    mSslSocket->setPrivateKey(mPrivateKey);
    mSslSocket->startServerEncryption();
    this->addPendingConnection(mSslSocket);
}

/**
 * Sets the server's certificate.
 *
 * @brief SslServer::setSslLocalCertificate
 * @param certificate
 */
void SslServer::setSslLocalCertificate(const QSslCertificate &certificate){
    this->mLocalCertificate = certificate;
}

/**
 * Sets the server's private asymmetric key.
 *
 * @brief SslServer::setSslPrivateKey
 * @param key
 */
void SslServer::setSslPrivateKey(const QSslKey &key){
    this->mPrivateKey = key;
}

/**
 * Set the allowed/supported protocols.
 *
 * Default: TLSv1 and SSLv3 but forces client to use TLS
 *
 * @brief SslServer::setSslProtocol
 * @param protocol
 */
void SslServer::setSslProtocol(QSsl::SslProtocol protocol){
    this->mProtocol = protocol;
}

/**
 * Set the checking of the peer's (server) certificate validity.
 *
 * Default: Verify that the server's certificate is valid
 *
 * @brief SslServer::setSslPeerVerifyMode
 * @param verifyMode
 */
void SslServer::setSslPeerVerifyMode(QSslSocket::PeerVerifyMode verifyMode){
    this->mPeerMode = verifyMode;
}

/**
 * Public slot that will handle SSL Socket generated errors.
 *
 * @brief SslServer::onSslErrors
 * @param errors
 */
void SslServer::onSslErrors(const QList <QSslError> & errors){
    // UNDER DEVELOPMENT
    qDebug() << "Errors with SSL";
}
