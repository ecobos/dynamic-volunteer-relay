#include "dynamicvolunteer.h"

#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

DynamicVolunteer::DynamicVolunteer(QObject *parent) : QObject(parent)
{
    connect(parent, SIGNAL(stop()), this, SLOT(stopConnections()));
    this->getAvailableSPfromCC();
}

void DynamicVolunteer::getAvailableSPfromCC(){
    // Get JSON config file
    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onConfigFileAttained(QNetworkReply*)));
    networkManager->get(QNetworkRequest(QUrl("http://karldotson.com/config.json")));
    networkManager->deleteLater();
}

void DynamicVolunteer::onConfigFileAttained(QNetworkReply* reply){
    if (reply->error() == QNetworkReply::NoError){
        QString data = (QString) reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
        QJsonObject jsonObject = jsonResponse.object();
        QString host = jsonObject["host"].toString();
        quint16 port = jsonObject["port"].toInt();
        if(!mClient){
            this->startClientServer();
        }
        mClient->connectToSPorFail(host, port);
    }else {
        qDebug() << "Something went wrong with getting config file from CC";
    }
    reply->deleteLater();
}

void DynamicVolunteer::startClientServer(){
    mClient = new ClientConnection(this);
    connect(mClient, SIGNAL(getSP()), this, SLOT(getAvailableSPfromCC()));
}

void DynamicVolunteer::stopConnections(){
    emit stop();
    delete mClient;
    mClient = NULL;
}
