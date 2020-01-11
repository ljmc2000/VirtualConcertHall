#include "httpapiclient.h"
#include "onlinestatusnamespace.h"
#include <QCoreApplication>

#define httpAPIurl "http://127.0.0.1:5000"
#define REQUEST    while(!reply->isFinished()) qApp->processEvents();\
    QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();\
    if (reply->error() != QNetworkReply::NoError) emit httpError(reply->error(),reply->errorString()); \
    else if (json["status"].toString() != "success") emit apiError(json["reason"].toString());\
    reply->deleteLater()

//common
bool HttpAPIClient::test()
{
    QJsonObject requestParams;
    QJsonObject json = postRequest("/test",requestParams);

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
    QJsonObject requestParams;
    QJsonObject json = postRequest("/logout",requestParams);
    token="";
    emit tokenChange();
}

QString HttpAPIClient::getUsername()
{
    QJsonObject requestParams;
    QJsonObject json = postRequest("/getUsername",requestParams);

    return json["status"].toString() == "success" ? json["username"].toString():"";
}

QString HttpAPIClient::createRoom(QString name, QString description, QString password, bool isprivate)
{
    QJsonObject requestParams;
    requestParams.insert("roomname",name);
    requestParams.insert("description",description);
    requestParams.insert("password",password);
    requestParams.insert("private",isprivate);
    QJsonObject json = postRequest("createRoom",requestParams);

    return json["roomId"].toString();
}

QList<RoomInfo> HttpAPIClient::listRooms()
{
    QJsonObject json = getRequest("/listRooms");

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

    return returnme;
}

RoomConnectionInfo HttpAPIClient::getCurrentRoom()
{
    QJsonObject request;
    QJsonObject json = postRequest("/getCurrentRoom",request);

    RoomConnectionInfo r;
    r.roomIp=json["roomIp"].toString();
    r.roomPort=json["roomPort"].toInt();
    r.secretId=json["secretId"].toInt();

    return r;
}

void HttpAPIClient::joinRoom(QString roomId)
{
    QJsonObject request;
    request.insert("roomId",roomId);
    postRequest("joinRoom",request);
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

    return json["clientId"].toString().toUInt();
}
#endif

QJsonObject HttpAPIClient::getRequest(QString endpoint)
{
    QNetworkRequest request(httpAPIurl+endpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply=netman.get(request);
    REQUEST;

    return json;
}

QJsonObject HttpAPIClient::postRequest(QString endpoint, QJsonObject requestParams)
{
    QNetworkRequest request(httpAPIurl+endpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    requestParams.insert("token",token);
    QNetworkReply *reply=netman.post(request,QJsonDocument(requestParams).toJson());
    REQUEST;

    return json;
}
