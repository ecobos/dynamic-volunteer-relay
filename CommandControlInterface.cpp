#include "CommandControlInterface.h"

#include <QNetworkRequest>
#include <QUrlQuery>
#include <QEventLoop>
#include <QJsonDocument>

const QString CommandControlInterface::LOGIN_ENDPOINT = "https://api.censorbuster.com/auth/login";
const QString CommandControlInterface::DVP_ENDPOINT = "https://api.censorbuster.com/api/dvp";
const QString CommandControlInterface::SP_ENDPOINT = "https://api.censorbuster.com/api/sp";
const QString CommandControlInterface::ONLINE_ENDPOINT = "https://api.censorbuster.com/api/dvp/online";
const QString CommandControlInterface::OFFLINE_ENDPOINT = "https://api.censorbuster.com/api/dvp/offline";

CommandControlInterface::CommandControlInterface(QObject *parent) : QObject(parent)
{
    mNetworkManager = new QNetworkAccessManager(this);
}

/**
 *
 * @brief CommandControlInterface::login
 * @return QString the login token
 */
void CommandControlInterface::login()
{
    if(mUUID == NULL || mPassword == NULL)
    {
        this->shutdown("Need to set UUID and Password");
    }

    qDebug() << "Contacting Command Control API";

    QUrl url(LOGIN_ENDPOINT);
    QNetworkRequest request( url );
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("Cache-Control", "no-cache");

    QUrlQuery postData;
    postData.addQueryItem("uuid", mUUID);
    postData.addQueryItem("password", QUrl::toPercentEncoding(mPassword));

    QNetworkReply *response = mNetworkManager->post(request, postData.toString(QUrl::FullyEncoded).toUtf8());
    connect(response, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(requestError(QNetworkReply::NetworkError)) );

    // Wait until we get a response
    QEventLoop loop;
    connect(response, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    qDebug() << "Got a response from login";
    if (response->error() == QNetworkReply::NoError){
        QString data = (QString) response->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
        QJsonObject jsonObject = jsonResponse.object();

        mToken = jsonObject["token"].toString();
        qDebug() << ">>> Token => "<< mToken;
    }
}

/**
 * @brief CommandControlInterface::status
 * @param newStatus new DVP status {ONLINE, OFFLINE}
 */
void CommandControlInterface::status(Status newStatus)
{
    if(mToken == NULL) { this->login(); }

    QUrl url;
    QString status_text;
    if (newStatus == ONLINE)
    {
        url.setUrl(ONLINE_ENDPOINT); status_text = "Online";
    }
    else
    {
        url.setUrl(OFFLINE_ENDPOINT); status_text = "Offline";
    }
    qDebug() << "Contacting Command Control API: Going " + status_text;

    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("Cache-Control", "no-cache");
    request.setRawHeader("Authorization", "Bearer "+ mToken.toUtf8());

    // Place request
    QNetworkReply *response = mNetworkManager->put(request, QByteArray() );
    connect(response, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(requestError(QNetworkReply::NetworkError)) );

    QEventLoop loop;
    connect(response, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    int httpStatus = response->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
}

QString CommandControlInterface::getAvailableStaticProxy()
{
    qDebug() << "Contacting Command Control API: Getting available Static Proxy";
    QUrl url(SP_ENDPOINT);
    QNetworkRequest request(url);
    request.setRawHeader("Cache-Control", "no-cache");
    request.setRawHeader("Authorization", "Bearer "+ mToken.toUtf8());
    QNetworkReply *response = mNetworkManager->get(request);
    connect(response, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(requestError(QNetworkReply::NetworkError)) );

    //quint8 httpStatus = response->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    QEventLoop loop;
    connect(response, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    qDebug() << "Got a reply regarding SPs";
    QString data = (QString) response->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject jsonObject = jsonResponse.object();

    QString host = jsonObject["ip"].toString();
    if(host.isEmpty())
    {
        this->shutdown("Json contains empty IP field");
    }
    qDebug() << ">>> " << host;
    return host;
}

QJsonObject* CommandControlInterface::registerApplication()
{
    qDebug() << "Contacting Command Control API: Register";

    QUrl url(CommandControlInterface::DVP_ENDPOINT);
    QNetworkRequest request( url );
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("Cache-Control", "no-cache");
    QNetworkReply *response = mNetworkManager->post(request, QByteArray());
    connect(response, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(requestError(QNetworkReply::NetworkError)) );

    // Wait until we get a response
    QEventLoop loop;
    connect(response, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    QString data = (QString) response->readAll();
    qDebug() << data;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject* jsonObject = new QJsonObject(jsonResponse.object());

    return jsonObject;
}

void CommandControlInterface::requestError(QNetworkReply::NetworkError)
{
    this->shutdown("Network Issue");
}

void CommandControlInterface::setCredentials(const QString & uuid, const QString & password)
{
    this->mUUID = uuid;
    this->mPassword = password;
    qDebug() << "Credentials Set";
}

void CommandControlInterface::shutdown(const QString & reason){
    qDebug() << "Shutting down Command Control Interface:: "+ reason;
    exit(0);
}

CommandControlInterface::~CommandControlInterface()
{
    // If token expires, we get a 401 HTTP status {"error": "token_expired" }
    this->status(OFFLINE);
    qDebug() << "Offline. BYE";
}
