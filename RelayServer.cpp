#include <Qfile>
#include "RelayServer.h"

RelayServer::RelayServer(QObject *parent) : QObject(parent){
    mSslServer = new SslServer(this);

    /*
     * .crt file: Is a .pem (or rarely .der) formatted file with a different extension,
     *  one that is recognized by Windows Explorer as a certificate,
     *  which .pem is not.
     */
    QFile certFile("dvp_cert_cc_signed.cert.pem");
    //certFile.open(QIODevice::ReadOnly);
    if(!certFile.open(QIODevice::ReadOnly ))
    {
        qDebug() << "Could not open file for reading";
    }
    QSslCertificate certificate( &certFile, QSsl::Pem );
    certFile.close();

    /*
     * .key file: Is a PEM formatted file containing just the private-key of a specific
     *  certificate and is merely a conventional name and not a standardized one. The
     *  rights on these files are very important, and some programs will refuse to
     *  load these certificates if they are set wrong.
     */
    QFile keyFile("dvp_cert_cc_signed.key.pem");
    //keyFile.open(QIODevice::ReadOnly);
    if(!keyFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Could not open file for reading";
    }
    QSslKey sslKey( &keyFile, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);
    keyFile.close();

    mSslServer->setSslLocalCertificate( certificate );
    mSslServer->setSslPrivateKey( sslKey );

    /*
     * We want to find the protocol support sweet spot by supporting
     *  older protocols but enforcing the newer TLS protocol.
     *
     * Ref: http://doc.qt.io/qt-5/qssl.html#SslProtocol-enum
     */
    mSslServer->setSslProtocol( QSsl::TlsV1_0 );


    /*
     * This setting does not apply to a server but I am leaving it
     *  just as a note of something that will need to be considered
     *  in the future development of this program. Eventually it should
     *  be set to QSslSocket::VerifyPeer as opposed to the default value
     *  of QSslSocket::QueryPeer.
     *
     *  Ref: http://doc.qt.io/qt-5/qsslsocket.html#PeerVerifyMode-enum
     */
    //mSslServer->setSslPeerVerifyMode( QSslSocket::VerifyNone );

    // newConnection() is a signal emitted by SslServer Class and
    //  is inherited from the QTcpServer Class
    connect(mSslServer, SIGNAL(newConnection()), this, SLOT(acceptNewConnection()));

    // Listen in on any address accessible through the local interfaces.
    //  This will likely be localhost (127.0.0.1:8080)
    if(!mSslServer->listen(QHostAddress::Any, 8080)){
        qDebug() << "Relay Server could not start :(";
    }
    else{
        qDebug() << "Relay Server started and listening...";
    }
}

/**
 * Sets the host and port that will be used to connect to the static proxy
 * @brief ClientConnection::setSP
 * @param host IP of the host
 * @param port Port number of the host
 */
void RelayServer::setStaticProxy(const QString & host, quint16 port){
    mSP_Port = port;
    mSP_Host = host;
}

/**
 * Attempt to connect to the static proxy. Fail if not able.
 * @brief ClientConnection::connectToSPorFail
 */
void RelayServer::connectToSPorFail(){
    qDebug() << "Connecting to SP";
    mSPConnection = new StaticProxyConnection(this);
    if(mSP_Host == NULL || mSP_Port == 0 ){
        emit getStaticProxy();
        return;
    }
    mSPConnection->connectTo(mSP_Host, mSP_Port);
    if(!mSPConnection->isOpen()){
        qDebug() << "SP connection failed";
    }
    connect(mSPConnection, SIGNAL(doRead()), this, SLOT(serverToClientWrite()));
}

/**
 * Accepts the next pending SSL socket conenction.
 * @brief ClientConnection::acceptNewConnection
 */
void RelayServer::acceptNewConnection(){
    //Get the next pending connection
    mClientSocket = (QSslSocket *) mSslServer->nextPendingConnection();

    connect(mClientSocket, SIGNAL(encrypted()), this, SLOT(handshakeSuccessful()));
    connect(mClientSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(onSslErrors(QList<QSslError>)));
    connect(mClientSocket, SIGNAL(peerVerifyError(QSslError)), this, SLOT(onPeerVerifyError(QSslError)));

    mClientSocket->startServerEncryption();

    //this->connectToSPorFail();
    //connect(mClientSocket, SIGNAL(readyRead()), this, SLOT(clientToServerWrite()));
    //qDebug() << "Connection encrypted? " << mClientSocket->isEncrypted();

}

void RelayServer::handshakeSuccessful()
{
    qDebug() << "New client connection accepted" << mClientSocket->isEncrypted();
    mClientSocket->write("\nHello there\r\n\r\n");
}

/**
 * [Slot] Handles the client socket's readyRead signal
 * @brief ClientConnection::clientToServerWrite
 */
void RelayServer::clientToServerWrite(){
    qDebug() << "[Client] Received " << mClientSocket->bytesAvailable() << "bytes from client. Sending to server...";

    // From Docs: QByteArray QIODevice::read(qint64 maxSize)
    QByteArray data = mClientSocket->read(mClientSocket->bytesAvailable());

    qDebug() << "[Client] Actually read " << data.length() << " from client";
    // From Docs: qint64 QIODevice::write(const QByteArray & byteArray)
    mSPConnection->write(data);
}

/**
 * [Slot] Handle the server socket's readyRead signal
 * @brief ClientConnection::serverToClientWrite
 */
void RelayServer::serverToClientWrite(){
    qDebug() << "[Client] Received " << mSPConnection->bytesAvailable() << "bytes from server. Sending to client...";

    QByteArray data = mSPConnection->readBytesAvailable();
    qDebug() << "[Client] Actually read " << data.length() << " from server";
    mClientSocket->write(data);
}

/**
 * Public slot that will handle SSL Socket generated errors.
 *
 * @brief SslServer::onSslErrors
 * @param errors
 */
void RelayServer::onSslErrors(const QList <QSslError> & errors)
{
    qDebug() << "Errors with SSL";
    foreach (const QSslError &error, errors)
    {
        qDebug() << error.errorString();
    }

}

void RelayServer::onPeerVerifyError(const QSslError & error)
{
    qDebug() << error.errorString();
}

/**
 * Stops the SSL Server from listening on the current address and port
 * @brief ClientConnection::stopListening
 */
void RelayServer::stopListening(){
    if(mSslServer->isListening()){
        //mSslServer->close();
        qDebug() << "stopped listening";
    }
    // Delete the SSL server object and clean up
    mSslServer->deleteLater();
    mSslServer = NULL;
}

