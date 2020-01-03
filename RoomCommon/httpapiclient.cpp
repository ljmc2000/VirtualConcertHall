#include "httpapiclient.h"
#include <QApplication>

#define REQUEST    while(!reply->isFinished()) qApp->processEvents();\
    QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();\
    if (reply->error() != QNetworkReply::NoError) emit apiError("http error: "+reply->errorString()); \
    if (json["status"].toString() != "success") emit apiError(json["reason"].toString());\
    reply->deleteLater()

HttpAPIClient::HttpAPIClient()
{

}

bool HttpAPIClient::test()
{
    QJsonObject requestParams;
    QJsonObject json = postRequest("/test",requestParams);

    return json["status"].toString() == "success";
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
        prefs.setValue("loginToken",json["token"].toString());
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

void HttpAPIClient::closeRoom()
{
    QJsonObject requestParams;
    postRequest("/closeRoom",requestParams);
}

QJsonObject HttpAPIClient::getRequest(QString endpoint)
{
    QNetworkRequest request(RoomCommon::httpAPIurl+endpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply=netman.get(request);
    REQUEST;

    return json;
}

QJsonObject HttpAPIClient::postRequest(QString endpoint, QJsonObject requestParams)
{
    QNetworkRequest request(RoomCommon::httpAPIurl+endpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    requestParams.insert("token",prefs.value("loginToken").toString());
    QNetworkReply *reply=netman.post(request,QJsonDocument(requestParams).toJson());
    REQUEST;

    return json;
}
