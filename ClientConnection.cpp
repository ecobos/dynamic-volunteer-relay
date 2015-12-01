/*
 * Edgar A. Cobos
 * CSULB Fall 2015
 * CECS 476 - Computer Security
 */

#include <Qfile>
#include "ClientConnection.h"


ClientConnection::ClientConnection(QObject *parent) : QObject(parent)
{    
    mSslServer = new SslServer(this);

    /*
     * .crt file: Is a .pem (or rarely .der) formatted file with a different extension,
     *  one that is recognized by Windows Explorer as a certificate,
     *  which .pem is not.
     */
    QFile certFile(":/certs/server.crt");
    certFile.open(QIODevice::ReadOnly);
    QSslCertificate certificate( &certFile, QSsl::Pem );
    certFile.close();

    /*
     * .key file: Is a PEM formatted file containing just the private-key of a specific
     *  certificate and is merely a conventional name and not a standardized one. The
     *  rights on these files are very important, and some programs will refuse to
     *  load these certificates if they are set wrong.
     */
    QFile keyFile(":/certs/server.key");  
    keyFile.open(QIODevice::ReadOnly);   
    QSslKey sslKey( &keyFile, QSsl::Rsa, QSsl::Pem );
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
    mSslServer->setSslPeerVerifyMode( QSslSocket::VerifyNone );

    // newConnection() is a signal emitted by SslServer Class and
    //  is inherited from the QTcpServer Class
    connect(mSslServer, SIGNAL(newConnection()), this, SLOT(acceptNewConnection()));

    // Listen in on any address accessible through the local interfaces.
    //  This will likely be localhost (127.0.0.1:8080)
    if(!mSslServer->listen(QHostAddress::Any, 8080))
    {
        qDebug() << "Server could not start";
    }
    else
    {
        qDebug() << "Client server started and listening.";
    }
}

void ClientConnection::setSP(const QString & host, quint16 port){
    mSP_Port = port;
    mSP_Host = host;
}

void ClientConnection::connectToSPorFail(){
    qDebug() << "Connecting to SP";
    mSPConnection = new StaticProxyConnection(this);
    mSPConnection->connectTo(mSP_Host, mSP_Port);
    if(!mSPConnection->isOpen()){
        qDebug() << "SP connection failed";
    }

    connect(mSPConnection, SIGNAL(readyRead()), this, SLOT(serverToClientWrite()));
}

/* -------------------------
 *          SLOTS
 * -------------------------
 */

/**
 * Accepts the next pending SSL socket conenction.
 *
 * @brief ClientConnection::acceptNewConnection
 */
void ClientConnection::acceptNewConnection()
{
    /*
     * Get the next pending connection
     */
    mClientSocket = mSslServer->nextPendingConnection();
    qDebug() << "New client connection accepted";
    this->connectToSPorFail();
    connect(mClientSocket, SIGNAL(readyRead()), this, SLOT(clientToServerWrite()));
}

void ClientConnection::clientToServerWrite(){
    qDebug() << "Received " << mClientSocket->bytesAvailable() << "bytes from client";
    qDebug() << "Sending to server...";

    // From Docs: QByteArray QIODevice::read(qint64 maxSize)
    QByteArray data = mClientSocket->read(mClientSocket->bytesAvailable());

    // From Docs: qint64 QIODevice::write(const QByteArray & byteArray)
    mSPConnection->write(data);
}

void ClientConnection::serverToClientWrite(){
    qDebug() << "Received " << mSPConnection->bytesAvailable() << "bytes from server";
    qDebug() << "Sending to client...";
    QByteArray data = mSPConnection->read();
    mClientSocket->write(data);
}

/**
 * Stops the SSL Server from listening on the current address and port
 *
 * @brief ClientConnection::stopListening
 */
void ClientConnection::stopListening(){
    if(mSslServer->isListening()){
        mSslServer->close();
        qDebug() << "stopped listening";
    }
    mSslServer->deleteLater(); // Delete the SSL server object
    mSslServer = NULL;
}

