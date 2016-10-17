#include "Controller.h"
#include <QJsonObject>
#include <QFile>
#include <QSettings>
#include <QEventLoop>
#include <QFileInfo>

Controller::Controller(QObject *parent) : QObject(parent)
{
    mRelay = NULL;
    connect(parent, SIGNAL(stop()), this, SLOT(stopConnections()));

}

void Controller::startUp()
{
    mCommandControl = new CommandControlInterface(this);
    Credentials config = this->loadConfiguration();
    mCommandControl->setCredentials(config.uuid, config.password);
    mCommandControl->login();
    this->startRelayServer(mCommandControl->getAvailableStaticProxy());
    mCommandControl->status(ONLINE);
}

Credentials Controller::loadConfiguration()
{
    QSettings config("CensorBuster", "DynamicVolunteerRelay");

    // If the UUID or Password are not set, we need to create them
    if(!config.contains("uuid") || !config.contains("password"))
    {
        this->getCredentials();
    }

    Credentials credentials;
    credentials.uuid = config.value("uuid").toString();
    credentials.password = config.value("password").toString();
    qDebug() << "Loading saved "<< credentials.uuid <<" "<< credentials.password;

    return credentials;
}

void Controller::getCredentials()
{
   QJsonObject* jsonObject = mCommandControl->registerApplication();
   QSettings config("CensorBuster", "DynamicVolunteerRelay");
   config.setValue("uuid", jsonObject->value("uuid").toString());
   config.setValue("password", jsonObject->value("plaintext_password").toString());
   qDebug() << "Assigned: " << jsonObject->value("uuid").toString() << " "<< jsonObject->value("plaintext_password").toString();
   this->saveX509(jsonObject->value("x509"));

   delete jsonObject;
}


void Controller::saveX509(QJsonValue jsonValue){

    // SAVE CERTIFICATE
    QFile cert_file("dvp_cert_cc_signed.cert.pem");
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
    QFile key_file("dvp_cert_cc_signed.key.pem");
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
 * -- UNUSED --
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
            this->startRelayServer(mCommandControl->getAvailableStaticProxy());
        }
        mRelay->setStaticProxy(host, 8081);
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
void Controller::startRelayServer(const QString & hostStaticProxy_IP){
    if(mRelay == NULL)
    {
        mRelay = new RelayServer(this);
    }
    qDebug() << "SP ip" << hostStaticProxy_IP;
    //Uncommend here for production
    mRelay->setStaticProxy(hostStaticProxy_IP, 8080);

    //For local testing
    //mRelay->setStaticProxy("127.0.0.1", 8081);
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
