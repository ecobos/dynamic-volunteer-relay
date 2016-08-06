#include "StaticProxyConnection.h"
#include <QHostAddress>
#include <QtNetwork/QSslCipher>

StaticProxyConnection::StaticProxyConnection(QObject *parent): QObject(parent)
{
    mTotalBytesWritten = 0;
    mSocket = new QTcpSocket(this); // QSslSocket
    connect(mSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
    //connect(mSocket, SIGNAL(encrypted()), this, SLOT(socketEncrypted()));
    connect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(mSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrors(QList<QSslError>)));
    //connect(mSocket, SIGNAL(encryptedBytesWritten(qint64)), this, SLOT(addToEncryptBytesWritten(qint64)));
    connect(mSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(mSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(parent, SIGNAL(stop()), this, SLOT(stop()));

    // Certificte pinning. This tells Qt that it should trust the specified certificate
    QList<QSslCertificate> pseudoTrustedCA = QSslCertificate::fromPath(":/certs/SP-server-certificate.pem");
    if (pseudoTrustedCA.empty()) {
        qFatal("Error: No Trusted Certificate Authorities");
    }
    //mSocket->setCaCertificates(pseudoTrustedCA);

    // Specify that we are only using the TLSv1 protocol
    //mSocket->setProtocol(QSsl::TlsV1_0);
}

/**
 * Makes a connection to the static proxy.
 * @brief StaticProxyConnection::connectTo
 * @param host
 * @param port
 */
void StaticProxyConnection::connectTo(const QString &host, quint16 port){
    qDebug() << "Connecting...";

    // This call is non-blocking, therefore we will need to wait for the connection
//    mSocket->connectToHostEncrypted(host, port);
//    if(mSocket->waitForEncrypted(1000)){
//        qDebug() << "Encrypted link successful";
//    }

    mSocket->connectToHost(host,port);
    if(mSocket->waitForConnected(1000)){
        qDebug("Can't connect");
    }
}

/**
 * Read from the encrypted socket.
 * @brief StaticProxyConnection::read
 * @return byte array containg the received data
 */
QByteArray StaticProxyConnection::readBytesAvailable(){
    QByteArray incomingData;
    if(mSocketState == QAbstractSocket::ConnectedState /*&& mSocket->isEncrypted()*/ ){

        incomingData = mSocket->read(mSocket->bytesAvailable());
        //mSocket->waitForReadyRead();
        qDebug()<< "[SP] Reading" << incomingData;
    }else{
        qDebug("[SP] Can't Read");
    }
    // TODO: This may return null but okay for now.
    return incomingData;
}

/**
 * Write to the encrypted socket.
 * @brief StaticProxyConnection::write
 * @param dataToWrite
 */
void StaticProxyConnection::write(const QByteArray &dataToWrite){
    if(mSocketState == QAbstractSocket::ConnectedState /*&& mSocket->isEncrypted() */){
        qDebug() << "[SP] Writting " << dataToWrite;
        qDebug() << "Wrote: " << mSocket->write(dataToWrite, dataToWrite.length());
        //mSocket->waitForBytesWritten();
        //mSocket->flush();
    }else{
        qDebug("Socket not encrypted yet");
    }
}

/**
 * Gets the number of bytes available in the socket buffer to be read.
 * @brief StaticProxyConnection::bytesAvailable
 * @return number of bytes currently in the socket buffer
 */
qint64 StaticProxyConnection::bytesAvailable(){
    return mSocket->bytesAvailable();
}

/**
 * Check if the socket is currently open.
 * @brief StaticProxyConnection::isOpen
 * @return true if the socket is open, false otherwise.
 */
bool StaticProxyConnection::isOpen(){
    return mSocket->isOpen();
}

/** --- SIGNALS --- **/

void StaticProxyConnection::readyRead(){
    qDebug("doRead() emitted");
    emit doRead();
}


/** --- SLOTS --- **/
void StaticProxyConnection::socketStateChanged(QAbstractSocket::SocketState state){
    mSocketState = state;
}

/**
 * [Slot] Called when the socket makes a connection, however the socket
 * is not yet encrypted.
 * @brief StaticProxyConnection::connected
 */
void StaticProxyConnection::connected(){
    qDebug() << "Connected. Waiting for an encrypted connection.";

}

/**
 * [Slot] Called when the socket connection has been upgraded to an
 * encrypted connection.
 * @brief StaticProxyConnection::socketEncrypted
 */
void StaticProxyConnection::socketEncrypted(){
    qDebug() << "Connection encrypted!";
    mSocket->setSocketOption(QAbstractSocket::KeepAliveOption, true);
    // Code below is only used for debugging purposes
//    QSslCipher sessionCipher = mSocket->sessionCipher();
//    QString availableCipher = QString("Available Ciphers > Auth:%1, Cipher:%2 ( Used:%3 / Supported:%4 )")
//                        .arg(sessionCipher.authenticationMethod())
//                        .arg(sessionCipher.name()).arg(sessionCipher.usedBits()).arg(sessionCipher.supportedBits());;
//    qDebug() << availableCipher;
}

void StaticProxyConnection::addToEncryptBytesWritten(qint64 bytes){
    //mTotalBytesWritten += bytes;
    qDebug() << "[SP] " << bytes << " encrypted bytes written.";
}

void StaticProxyConnection::socketError(QAbstractSocket::SocketError er){
    qDebug() << "[SP] Socket Error " << er << ": " << mSocket->socketDescriptor() << " "<< mSocket->errorString();

}

void StaticProxyConnection::sslErrors(const QList<QSslError> &errors){
    foreach (const QSslError &error, errors)
        qDebug() << "[SP] SSL Error on: " << error.errorString();

}

void StaticProxyConnection::stop(){
    // needs implementation
}

void StaticProxyConnection::disconnected(){
    qDebug() << "Disconnected...";
}
