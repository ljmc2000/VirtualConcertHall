#include "httpapiclient.h"
#include "onlinestatusnamespace.h"
#include <QCoreApplication>

#define httpAPIurl "http://virtualconcerthall.urown.cloud:5000"
#define REQUEST    while(!reply->isFinished()) qApp->processEvents();\
    QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();\
    if (reply->error() != QNetworkReply::NoError) emit httpError(reply->error(),reply->errorString()); \
    else if (json["status"].toString() != "success") emit apiError(json["reason"].toString());\
    reply->deleteLater()

//common
bool HttpAPIClient::test()
{
    QJsonObject requestParams;
    QJsonObject json = getRequest("/test");

    return !json["invalid"].toBool();
}

int HttpAPIClient::getUserStatus()
{
    QMetaEnum e = QMetaEnum::fromType<OnlineStatusNamespace::State>();
    QJsonObject requestParams;
    QJsonObject json = postRequest("/getUserStatus",requestParams);

    return e.keyToValue(json["userStatus"].toString().toStdString().c_str());
}


#ifdef QT_GUI_LIB   //client
HttpAPIClient::HttpAPIClient()
{
    this->token=prefs.value("loginToken").toString();
}

void HttpAPIClient::signup(QString username,QString password)
{
    QJsonObject requestParams;
    requestParams.insert("username",username);
    requestParams.insert("password",password);
    postRequest("/register",requestParams);
}

bool HttpAPIClient::signin(QString username,QString password)
{
    QJsonObject requestParams;
    requestParams.insert("username",username);
    requestParams.insert("password",password);
    QJsonObject json = postRequest("/login",requestParams);

    if(json["status"].toString() == "success")
    {
        QString t = json["token"].toString();
        prefs.setValue("loginToken",t);
        this->token=t;
        emit tokenChange();
        return true;
    }

    else
    {
        return false;
    }
}

void HttpAPIClient::signout()
{
    QJsonObject json = getRequest("/logout");
    token="";
    emit tokenChange();
}

QString HttpAPIClient::getUsername()
{
    QJsonObject json = getRequest("/getUsername");
    return json["status"].toString() == "success" ? json["username"].toString():"";
}

QString HttpAPIClient::createRoom(QString name, QString description, QString password, bool isprivate)
{
    QJsonObject requestParams;
    requestParams.insert("roomname",name);
    requestParams.insert("description",description);
    requestParams.insert("password",password);
    requestParams.insert("private",isprivate);
    QJsonObject json = postRequest("/createRoom",requestParams);

    if(json["status"].toString()=="success")
    {
        emit roomReady();
        return json["roomId"].toString();
    }

    else
    {
        return nullptr;
    }
}

QList<RoomInfo> HttpAPIClient::listRooms(int page, int perPage)
{
    QJsonObject request;
    request.insert("page",page);
    request.insert("perPage",perPage);

    QJsonObject json = postRequest("/listRooms",request);
    QList<RoomInfo> returnme;
    foreach(QJsonValue j, json["results"].toArray())
    {
        QJsonObject o = j.toObject();
        RoomInfo r;
        r.roomName=o["roomname"].toString();
        r.roomId=o["roomId"].toString();
        r.owner=o["owner"].toString();
        r.description=o["description"].toString();

        returnme.append(r);
    }
    emit more(json["more"].toBool());

    return returnme;
}

RoomConnectionInfo HttpAPIClient::getCurrentRoom()
{
    QJsonObject request;
    QJsonObject json = getRequest("/getCurrentRoom");

    RoomConnectionInfo r;
    r.roomIp=json["roomIp"].toString();
    r.roomPort=json["roomPort"].toInt();
    r.secretId=json["secretId"].toString().toUInt();

    return r;
}

void HttpAPIClient::joinRoom(QString roomId)
{
    QJsonObject request;
    request.insert("roomId",roomId);
    QJsonObject json=postRequest("/joinRoom",request);

    if(json["status"].toString()=="success") emit roomReady();
}

void HttpAPIClient::leaveRoom()
{
    QJsonObject request;
    postRequest("leaveRoom",request);
}

void HttpAPIClient::closeRoom()
{
    QJsonObject requestParams;
    postRequest("/closeRoom",requestParams);
}
#else   //server
HttpAPIClient::HttpAPIClient()
{
    this->token=qgetenv("TOKEN");
}

quint32 HttpAPIClient::getClientId(quint32 secretId)
{
    QJsonObject requestParams;
    requestParams.insert("secretId",QString::number(secretId));
    QJsonObject json = postRequest("/getClientId",requestParams);

    if(json["status"].toString()=="success")
        return json["clientId"].toString().toUInt();
    else
        return 0;
}

void HttpAPIClient::timeoutRoom()
{
    QJsonObject requestParams;
    QJsonObject json = postRequest("/timeoutRoom",requestParams);
}
#endif

QJsonObject HttpAPIClient::getRequest(QString endpoint)
{
    QNetworkRequest request(httpAPIurl+endpoint);
    request.setRawHeader("loginToken",token.toUtf8());
    QNetworkReply *reply=netman.get(request);
    REQUEST;

    return json;
}

QJsonObject HttpAPIClient::postRequest(QString endpoint, QJsonObject requestParams)
{
    QNetworkRequest request(httpAPIurl+endpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("loginToken",token.toUtf8());
    QNetworkReply *reply=netman.post(request,QJsonDocument(requestParams).toJson());
    REQUEST;

    return json;
}
