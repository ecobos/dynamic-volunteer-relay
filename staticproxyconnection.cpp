#include "staticproxyconnection.h"
#include <QHostAddress>

StaticProxyConnection::StaticProxyConnection(QObject *parent, const QString &host, quint16 port)
    : QObject(parent), mHost(host), mPort(port)
{

}

void StaticProxyConnection::startConnection(){
    mSocket = new QTcpSocket(this);
    connect(mSocket, SIGNAL(connected()), this, SLOT(connected()));
    connect(mSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(mSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    connect(mSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    qDebug() << "connecting...";

    // this is not blocking call
    mSocket->connectToHost(mHost, mPort);

    // we need to wait...
    if(!mSocket->waitForConnected(5000))
    {
        qDebug() << "Error: " << mSocket->errorString();
    }
}


void StaticProxyConnection::connected()
{
    qDebug() << "connected...";

    // Hey server, tell me about you.
    mSocket->write("HEAD / HTTP/1.0\r\n\r\n\r\n\r\n");
}

void StaticProxyConnection::disconnected()
{
    qDebug() << "disconnected...";
}

void StaticProxyConnection::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << " bytes written...";
}

void StaticProxyConnection::readyRead()
{
    qDebug() << "reading...";

    // read the data from the socket
    qDebug() << mSocket->readAll();
}
