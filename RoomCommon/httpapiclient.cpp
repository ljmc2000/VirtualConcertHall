#include "httpapiclient.h"
#include <QCoreApplication>

#define REQUEST    while(!reply->isFinished()) qApp->processEvents();\
    QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();\
    if (reply->error() != QNetworkReply::NoError) emit httpError(reply->error(),reply->errorString()); \
    else if (json["status"].toString() != "success") emit apiError(json["reason"].toString());\
    reply->deleteLater()

HttpAPIClient::HttpAPIClient()
{
    this->token=prefs.value("loginToken").toString();
}

HttpAPIClient::HttpAPIClient(QString token)
{
    this->token=token;
}

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

const QString HttpAPIClient::httpAPIurl="http://127.0.0.1:5000";

//common
bool HttpAPIClient::test()
{
    QJsonObject requestParams;
    QJsonObject json = postRequest("/test",requestParams);

    return !json["invalid"].toBool();
}

//client
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
        return true;
    }

    else
    {
        return false;
    }
}

QString HttpAPIClient::createRoom(QString name)
{
    QJsonObject requestParams;
    requestParams.insert("roomname",name);
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

//server
quint32 HttpAPIClient::getClientId(quint32 secretId)
{
    QJsonObject requestParams;
    requestParams.insert("secretId",QString::number(secretId));
    QJsonObject json = postRequest("/getClientId",requestParams);

    return json["clientId"].toString().toUInt();
}