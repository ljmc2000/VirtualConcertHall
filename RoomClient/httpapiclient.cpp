#include "httpapiclient.h"
#include <QApplication>

#define POSTREQUEST requestParams.insert("token",prefs.value("loginToken").toString()); QNetworkReply *reply=netman.post(request,QJsonDocument(requestParams).toJson()); REQUEST
#define GETREQUEST QNetworkReply *reply=netman.get(request); REQUEST

#define REQUEST    while(!reply->isFinished()) qApp->processEvents();\
    QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();\
    if (json["status"].toString() != "success") emit apiError(json["reason"].toString());\
    reply->deleteLater()

HttpAPIClient::HttpAPIClient()
{

}

bool HttpAPIClient::test()
{
    QNetworkRequest request = getRequest("/test");
    QJsonObject requestParams;
    POSTREQUEST;

    return json["status"].toString() == "success";
}

void HttpAPIClient::signup(QString username,QString password)
{
    QNetworkRequest request = getRequest("/register");
    QJsonObject requestParams;
    requestParams.insert("username",username);
    requestParams.insert("password",password);
    POSTREQUEST;
}

bool HttpAPIClient::signin(QString username,QString password)
{
    QNetworkRequest request = getRequest("/login");
    QJsonObject requestParams;
    requestParams.insert("username",username);
    requestParams.insert("password",password);
    POSTREQUEST;

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
    QNetworkRequest request = getRequest("/createRoom");
    QJsonObject requestParams;
    requestParams.insert("roomname",name);
    POSTREQUEST;

    return json["roomId"].toString();
}

QList<RoomInfo> HttpAPIClient::listRooms()
{
    QNetworkRequest request(RoomCommon::httpAPIurl+"/listRooms");
    GETREQUEST;

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
    QNetworkRequest request = getRequest("/closeRoom");
    QJsonObject requestParams;
    POSTREQUEST;
}

QNetworkRequest HttpAPIClient::getRequest(QString endpoint)
{
    QNetworkRequest request(RoomCommon::httpAPIurl+endpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    return request;
}
