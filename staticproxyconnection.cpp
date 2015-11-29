#include "staticproxyconnection.h"
#include <QHostAddress>
#include <QtNetwork/QSslCipher>


StaticProxyConnection::StaticProxyConnection(QObject *parent, const QString &host, quint16 port)
    : QObject(parent), mHost(host), mPort(port)
{
    mSocket = new QSslSocket(this);
    connect(mSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
    connect(mSocket, SIGNAL(encrypted()), this, SLOT(socketEncrypted()));
    connect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(mSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrors(QList<QSslError>)));
    connect(mSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    connect(mSocket, SIGNAL(readyRead()), this, SLOT(read()));
    connect(parent, SIGNAL(stop()), this, SLOT(stop()));


    QList<QSslCertificate> pseudoTrustedCA = QSslCertificate::fromPath(":/certs/SP-server-certificate.pem");
    if (pseudoTrustedCA.empty()) {
        qFatal("Error: No Trusted Certificate Authorities");
    }
    mSocket->setCaCertificates(pseudoTrustedCA);

    mSocket->setProtocol(QSsl::TlsV1_0);
}

void StaticProxyConnection::startConnection(){

    qDebug() << "Connecting...";

    // this is not blocking call
    mSocket->connectToHostEncrypted(mHost, mPort);

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

void StaticProxyConnection::socketStateChanged(QAbstractSocket::SocketState state){
    mSocketState = state;
    /*
    if(state == QAbstractSocket::ConnectedState){
        this->connected();
    } else if (state == QAbstractSocket::ClosingState || state == QAbstractSocket::UnconnectedState){
        //this->disconnected();
    }
    */
}

void StaticProxyConnection::connected()
{
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

void StaticProxyConnection::disconnect()
{
    qDebug() << "Disconnected.";
    if(mSocket != NULL){
        delete mSocket;
        mSocket = NULL;
    }
}

void StaticProxyConnection::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << " bytes written...";
}

QByteArray StaticProxyConnection::read()
{
    qDebug() << "Reading...";
    QByteArray received;
    if(mSocketState == QAbstractSocket::ConnectedState){
        received =  mSocket->readAll();
        // read the data from the socket
        qDebug() << QString::fromUtf8(received);
    } else {
        qDebug() << "read: socket not connected";
        this->startConnection();
    }

    return received;
}

void StaticProxyConnection::write(const QString dataToWrite){
    if(mSocketState == QAbstractSocket::ConnectedState){
        qDebug() << "Writting " << dataToWrite;
        mSocket->write(dataToWrite.toUtf8() + "\r\n\r\n");
    } else {
        qDebug() << "write: socket not connected";
        this->startConnection();
    }
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
