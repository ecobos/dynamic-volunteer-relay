Controller::Controller(QObject *parent) : QObject(parent){
    mClient = NULL;
    connect(parent, SIGNAL(stop()), this, SLOT(stopConnections()));
    getAvailableSPfromCC();
}

/**
 * Retrieves all available static proxies from an external server.
 * @brief Controller::getAvailableSPfromCC
 */
void Controller::getAvailableSPfromCC(){
    qDebug() << "Contacting SP";
    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onConfigFileAttained(QNetworkReply*)));
    networkManager->get(QNetworkRequest(QUrl("http://karldotson.com/config.json")));
}

/**
 * [Slot] Handles the network managers finished callback.
 * @brief Controller::onConfigFileAttained
 * @param reply result of the network request
 */
void Controller::onConfigFileAttained(QNetworkReply* reply){
    if (reply->error() == QNetworkReply::NoError){
        QString data = (QString) reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(data.toUtf8());
        QJsonObject jsonObject = jsonResponse.object();
        QString host = jsonObject["host"].toString();
        quint16 port = jsonObject["port"].toInt();
        if(mClient == NULL){
            this->startClientServer();
        }
        mClient->setSP(host, port);
        qDebug() << "Got SP config info from CC";
    }else {
        qDebug() << "Something went wrong with getting config file from CC";
    }
    // Clean up dynamic memory
    reply->deleteLater();
}

/**
 * Starts the client server.
 * @brief Controller::startClientServer
 */
void Controller::startClientServer(){
    qDebug() << "Creating ClientConnection object";
    mClient = new ClientConnection(this);
    connect(mClient, SIGNAL(getSP()), this, SLOT(getAvailableSPfromCC()));
}

/**
 * Stops all connections then does some memory cleaning
 * @brief Controller::stopConnections
 */
void Controller::stopConnections(){
    emit stop();
    delete mClient;
    mClient = NULL;
}
