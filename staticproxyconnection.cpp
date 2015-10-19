#include "staticproxyconnection.h"
#include <QHostAddress>
#include <QtNetwork/QSslCipher>


StaticProxyConnection::StaticProxyConnection(QObject *parent, const QString &host, quint16 port)
    : QObject(parent), mHost(host), mPort(port)
{
    // Nothing to see here.
}

void StaticProxyConnection::startConnection(){

    if (!mSocket) {
        mSocket = new QSslSocket(this);
        connect(mSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
        connect(mSocket, SIGNAL(encrypted()), this, SLOT(socketEncrypted()));
        connect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
        connect(mSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrors(QList<QSslError>)));
        //connect(mSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
        connect(mSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
        QList<QSslCertificate> pseudoTrustedCA = QSslCertificate::fromPath(":/certs/SP-server-certificate.pem");
        if (pseudoTrustedCA.empty()) {
            qFatal("Error: No Trusted Certificate Authorities");
        }
        mSocket->setCaCertificates(pseudoTrustedCA);
        mSocket->setProtocol(QSsl::TlsV1_0OrLater);

        qDebug() << "connecting...";

        // this is not blocking call
        mSocket->connectToHostEncrypted(mHost, mPort);

        // we need to wait...
        if(!mSocket->waitForConnected(50)){
            qDebug() << "Error: " << mSocket->errorString();
        }
    }
}

void StaticProxyConnection::socketStateChanged(QAbstractSocket::SocketState state){
    if(state == QAbstractSocket::ConnectedState){
        this->connected();
    } else if (state == QAbstractSocket::ClosingState || state == QAbstractSocket::UnconnectedState){
        this->disconnected();
    }
}

void StaticProxyConnection::connected()
{
    qDebug() << "connected.";

}

void StaticProxyConnection::socketEncrypted(){
    qDebug() << "connection encrypted...";
    QSslCipher ciph = mSocket->sessionCipher();
    QString cipher = QString("%1, %2 (%3/%4)").arg(ciph.authenticationMethod())
                        .arg(ciph.name()).arg(ciph.usedBits()).arg(ciph.supportedBits());;
    qDebug() << cipher;

    // Hey server
    QString input = "Ello from the volunteer";
    mSocket->write(input.toUtf8() + "\r\n\r\n");
}

void StaticProxyConnection::disconnected()
{
    qDebug() << "disconnected...";
    mSocket->deleteLater();
    mSocket = NULL;
}

void StaticProxyConnection::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << " bytes written...";
}

void StaticProxyConnection::readyRead()
{
    qDebug() << "reading...";

    // read the data from the socket
    qDebug() << QString::fromUtf8(mSocket->readAll());
}

void StaticProxyConnection::write(const QString dataToWrite){
    qDebug() << "Writting " << dataToWrite;

}

void StaticProxyConnection::socketError(QAbstractSocket::SocketError)
{
    qDebug() <<  mSocket->errorString();

}

void StaticProxyConnection::sslErrors(const QList<QSslError> &errors)
{
    foreach (const QSslError &error, errors)
        qDebug() << error.errorString();

}
