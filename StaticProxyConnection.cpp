#include "StaticProxyConnection.h"
#include <QHostAddress>
#include <QtNetwork/QSslCipher>


StaticProxyConnection::StaticProxyConnection(QObject *parent): QObject(parent)
{
    mSocket = new QSslSocket(this);
    connect(mSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
    connect(mSocket, SIGNAL(encrypted()), this, SLOT(socketEncrypted()));
    connect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(mSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrors(QList<QSslError>)));
    connect(mSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    connect(mSocket, SIGNAL(readyRead()), this, SLOT(readReady()));
    connect(parent, SIGNAL(stop()), this, SLOT(stop()));


    QList<QSslCertificate> pseudoTrustedCA = QSslCertificate::fromPath(":/certs/SP-server-certificate.pem");
    if (pseudoTrustedCA.empty()) {
        qFatal("Error: No Trusted Certificate Authorities");
    }
    mSocket->setCaCertificates(pseudoTrustedCA);

    mSocket->setProtocol(QSsl::TlsV1_0);
}

void StaticProxyConnection::connectTo(const QString &host, quint16 port){

    qDebug() << "Connecting...";

    // this is not blocking call
    mSocket->connectToHostEncrypted(host, port);

    // we need to wait...
    if(!mSocket->waitForConnected(50)){

        qDebug() << "Error: " << mSocket->errorString();
    }

}

bool StaticProxyConnection::ping(){
    qDebug() << "Ping SP...";

    // This is not blocking call
    mSocket->connectToHostEncrypted(mHost, mPort);

    // We will wait for the connection to succeed
    if(!mSocket->waitForConnected(50)){
        qDebug() << "Error: " << mSocket->errorString();
        return false;
    } else{
        // We are just pinging the server, no need to keep the connection open
        mSocket->close();
        return true;
    }
}

QByteArray StaticProxyConnection::read(){
    QByteArray data;
    if(mSocketState == QAbstractSocket::ConnectedState && mSocket->isEncrypted()){
        data = mSocket->read(mSocket->encryptedBytesAvailable());
    }

    // This may return null but okay for now.
    return data;
}

void StaticProxyConnection::write(const QByteArray &dataToWrite){
    if(mSocketState == QAbstractSocket::ConnectedState && mSocket->isEncrypted()){
        qDebug() << "Writting " << dataToWrite;
        mSocket->write(dataToWrite);
    }
}

qint64 StaticProxyConnection::bytesAvailable(){
    return mSocket->encryptedBytesAvailable();
}

bool StaticProxyConnection::isOpen(){
    return mSocket->isOpen();
}

void StaticProxyConnection::disconnect()
{
    qDebug() << "Disconnected.";
    if(mSocket != NULL){
        delete mSocket;
        mSocket = NULL;
    }
}

/** --- SIGNALS --- **/

void StaticProxyConnection::readReady()
{
    emit readyRead();
}


/** --- SLOTS --- **/
void StaticProxyConnection::socketStateChanged(QAbstractSocket::SocketState state){
    mSocketState = state;
}

void StaticProxyConnection::connected(){
    qDebug() << "Connected. Waiting for an encrypted connection.";

}

void StaticProxyConnection::socketEncrypted(){
    qDebug() << "Connection encrypted!";
    QSslCipher ciph = mSocket->sessionCipher();
    QString cipher = QString("This session -- Auth:%1, Cipher:%2 (Used:%3/ Supported:%4)").arg(ciph.authenticationMethod())
                        .arg(ciph.name()).arg(ciph.usedBits()).arg(ciph.supportedBits());;
    qDebug() << cipher;

    // Hey server
    QString input = "Ello from the volunteer";
    mSocket->write(input.toUtf8() + "\r\n\r\n");
}

void StaticProxyConnection::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << " bytes written...";
}

void StaticProxyConnection::socketError(QAbstractSocket::SocketError)
{
    qDebug() << "Socket Error: " << mSocket->errorString();

}

void StaticProxyConnection::sslErrors(const QList<QSslError> &errors)
{
    qDebug() << "SSL Error: ";
    foreach (const QSslError &error, errors)
        qDebug() << error.errorString();

}

void StaticProxyConnection::stop(){

}
