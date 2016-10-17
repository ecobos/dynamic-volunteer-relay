#include "StaticProxyConnection.h"
#include <QHostAddress>
#include <QtNetwork/QSslCipher>

StaticProxyConnection::StaticProxyConnection(QObject *parent): QObject(parent)
{
    mTotalBytesWritten = 0;
    mSocket = new QTcpSocket(this); // QSslSocket
    connect(mSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
    connect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));

    connect(mSocket, SIGNAL(readyRead()), parent, SLOT(serverToClientWrite()));

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

    mSocket->connectToHost(host,port);

    if(!mSocket->waitForConnected(2000)){
        qDebug("Can't connect to Static Proxy");
    } else {
        qDebug("Connected to Static Proxy");
    }

}

/**
 * Read from the encrypted socket.
 * @brief StaticProxyConnection::read
 * @return byte array containg the received data
 */
QByteArray StaticProxyConnection::readBytesAvailable(){
    return mSocket->read(mSocket->bytesAvailable());
}

/**
 * Write to the socket.
 * @brief StaticProxyConnection::write
 * @param dataToWrite
 */
void StaticProxyConnection::write(const QByteArray &dataToWrite){
    mSocket->write(dataToWrite);
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

/** --- SLOTS --- **/
void StaticProxyConnection::socketStateChanged(QAbstractSocket::SocketState state)
{
    mSocketState = state;
}

void StaticProxyConnection::socketError(QAbstractSocket::SocketError er)
{
    qDebug() << "[SP] Socket Error " << er << ": " << mSocket->socketDescriptor() << " "<< mSocket->errorString();
}

void StaticProxyConnection::stop()
{
    // needs implementation
}

void StaticProxyConnection::disconnected()
{
    qDebug() << "Disconnected...";
}
