#include "Controller.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QSettings>
#include <QEventLoop>

#include <QFileInfo>

Controller::Controller(QObject *parent) : QObject(parent)
{
    mRelay = NULL;
    connect(parent, SIGNAL(stop()), this, SLOT(stopConnections()));
    //getAvailableSPfromCC();
}

void Controller::login()
{
    this->loadConfiguration();

    qDebug() << "Contacting Command Control API: Login";
    QNetworkAccessManager *networkManager_login = new QNetworkAccessManager(this);
    QUrl url("https://api.censorbuster.com/auth/login");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QByteArray body("uuid=" + mUUID.toUtf8() + "&password=" + mPassword.toUtf8());
    QNetworkReply *response = networkManager_login->post(request,body);

    // Wait until we get a response
    QEventLoop loop;
    connect(response, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    qDebug() << "Got a response from login";
    if (response->error() == QNetworkReply::NoError){
        QString data = (QString) response->readAll();
        qDebug() << data;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
        QJsonObject jsonObject = jsonResponse.object();

        mToken = jsonObject["token"].toString();
        qDebug() << "Response: Token => "<< mToken;
    }else{
        qDebug() << ">> " << response->error() << response->errorString();
        exit(-1);
    }

    networkManager_login->deleteLater();
}

void Controller::goOnline()
{
    this->login();

    qDebug() << "Contacting Command Control API: Going Online";
    QNetworkAccessManager *networkManager_goOnline = new QNetworkAccessManager(this);
    QUrl url("https://api.censorbuster.com/api/dvp");
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("Cache-Control", "no-cache");
    request.setRawHeader("Authorization", "Bearer "+ mToken.toUtf8());

    QByteArray body("is_available=1");

    // Place request
    QNetworkReply *response = networkManager_goOnline->put(request,body);

    // Wait until we get a response
    QEventLoop loop;
    connect(response, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    // Response
    qDebug() << "Got a response from trying to go online";
    if (response->error() == QNetworkReply::NoError){
        QString data = (QString) response->readAll();
        qDebug() << data;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
        QJsonObject jsonObject = jsonResponse.object();

        QString availability = jsonObject["is_available"].toString();
        qDebug() << ">> is_available => "<< availability;
    }else{
        qDebug() << ">> " << response->error();
        return;
    }

    networkManager_goOnline->deleteLater();
    this->getAvailableStaticProxies();
}

void Controller::loadConfiguration()
{
    QSettings config("CensorBuster", "DynamicVolunteerRelay");

    // If the UUID or Password are not set, we need to create them
    if(!config.contains("uuid") || !config.contains("password"))
    {
        this->getConfiguration();
    }

    mUUID = config.value("uuid").toString();
    mPassword = config.value("password").toString();
    qDebug() << "Loading saved "<< mUUID <<" "<< mPassword;
}

void Controller::getConfiguration()
{
    qDebug() << "Contacting Command Control API: Register";
    QNetworkAccessManager *networkManager_register = new QNetworkAccessManager(this);
    //connect(networkManager_register, SIGNAL(finished(QNetworkReply*)), this, SLOT(saveConfiguration(QNetworkReply*)));
    QUrl url("https://api.censorbuster.com/api/dvp");
    QNetworkRequest request(url);
    QByteArray params;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *response = networkManager_register->post(request, params);

    // Wait until we get a response
    QEventLoop loop;
    connect(response, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    this->saveConfiguration(response);

    networkManager_register->deleteLater();
}

void Controller::saveConfiguration(QNetworkReply* response)
{
    qDebug() << "Got a response from register";
    if (response->error() == QNetworkReply::NoError){
        QString data = (QString) response->readAll();
        qDebug() << data;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
        QJsonObject jsonObject = jsonResponse.object();

        qDebug() << "creating config";
        QSettings config("CensorBuster", "DynamicVolunteerRelay");
        config.setValue("uuid", jsonObject["uuid"].toString());
        config.setValue("password", jsonObject["plaintext_password"].toString());
        qDebug() << "Assigned: " << jsonObject["uuid"].toString() << " "<< jsonObject["plaintext_password"].toString();
        this->saveX509(jsonObject.value("x509"));

    }else{
        qDebug() << response->error();
    }
      response->deleteLater();
}

void Controller::saveX509(QJsonValue jsonValue){

    // SAVE CERTIFICATE
    QFile cert_file("dvp_cert_cc_signed.crt");
    if(!cert_file.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug() << "Could not open file for writing";
        qDebug() << "exists?              " << cert_file.exists();
        qDebug() << "writable?            " << cert_file.isWritable();
        qDebug() << "permissions before?  " << cert_file.permissions();
        qDebug() << "permissions set?     " << cert_file.setPermissions(QFileDevice::WriteOther | QFileDevice::ReadOther);
        qDebug() << "permissions after?   " << cert_file.permissions();
        qDebug() << "opened?              " << cert_file.open(QIODevice::Append);
        qDebug() << "errors?              " << cert_file.errorString();
        qDebug() << "errnum?              " << cert_file.error();
        return;
    }
    QFileInfo finfo(cert_file);
    qDebug() << finfo.absoluteFilePath();

    QTextStream toSave(&cert_file);
    toSave << jsonValue.toObject().value("x509cert").toString();
    cert_file.flush();
    cert_file.close();

    // SAVE CERTIFICATE KEY
    QFile key_file("dvp_cert_cc_signed.key");
    if(!key_file.open(QFile::WriteOnly | QFile::Text))
    {
        qDebug() << "Could not open key file for writing";
        return;
    }
    QFileInfo finfo_key(key_file);
    qDebug() << finfo_key.absoluteFilePath();

    QTextStream toSave_key(&key_file);
    toSave_key << jsonValue.toObject().value("privateKey").toString();
    key_file.flush();
    key_file.close();
}

/**
 * Retrieves all available static proxies from an external server.
 * @brief Controller::getAvailableSPfromCC
 */
void Controller::getAvailableStaticProxies(){
    qDebug() << "Contacting Command Control API: Getting available Static Proxies";
    QNetworkAccessManager *networkManager_getSPs = new QNetworkAccessManager(this);
    connect(networkManager_getSPs, SIGNAL(finished(QNetworkReply*)), this, SLOT(onStaticProxyAttained(QNetworkReply*)));

    QUrl url("https://api.censorbuster.com/api/sp");
    QNetworkRequest request(url);
    request.setRawHeader("Cache-Control", "no-cache");
    request.setRawHeader("Authorization", "Bearer "+ mToken.toUtf8());
    networkManager_getSPs->get(request);

    //networkManager_getSPs->deleteLater();
}

/**
 * [Slot] Handles the network managers finished callback.
 * @brief Controller::onSPsAttained
 * @param reply result of the network request
 */
void Controller::onStaticProxyAttained(QNetworkReply* reply){
    qDebug() << "Got a reply regarding SPs";
    if (reply->error() == QNetworkReply::NoError){
        QString data = (QString) reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
        QJsonObject jsonObject = jsonResponse.object();

        QString host = jsonObject["ip"].toString();
        //quint16 port = jsonObject["port"].toInt();
        qDebug() << "Got Static Proxy: " << host;
        if(mRelay == NULL){
            this->startRelayServer();
        }
        mRelay->setStaticProxy(host, 443);
        qDebug() << "Got Static Proxy configuration information from Command Control";
    } else {
        qDebug() << "Something went wrong with getting config file from CC";
    }

    // Clean up
    reply->deleteLater();
}

/**
 * Starts the client server.
 * @brief Controller::startClientServer
 */
void Controller::startRelayServer(){
    qDebug() << "Creating ClientConnection object";
    mRelay = new RelayServer(this);
    connect(mRelay, &RelayServer::getStaticProxy, this, &Controller::getAvailableStaticProxies);
}

/**
 * Stops all connections then does some memory cleaning
 * @brief Controller::stopConnections
 */
void Controller::stopConnections(){
    emit stop();
    delete mRelay;
    mRelay = NULL;
}
