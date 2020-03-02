#include "httpapiclient.h"
#include "onlinestatusnamespace.h"

#include <QCoreApplication>

#define HTTPAPIURL "http://virtualconcerthall.uksouth.azurecontainer.io"
//#define HTTPAPIURL "http://virtualconcerthall.urown.cloud:5000"

#define REQUEST    while(!reply->isFinished()) qApp->processEvents();\
    QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();\
    if (reply->error() != QNetworkReply::NoError) emit httpError(reply->error(),reply->errorString()); \
    else if (json["status"].toString() != "success") emit apiError(json["reason"].toString());\
    reply->deleteLater()

QMetaEnum HttpAPIClient::MetaStopReason = QMetaEnum::fromType<HttpAPIClient::StopReason>();
QMetaEnum HttpAPIClient::MetaUpdateType = QMetaEnum::fromType<HttpAPIClient::UpdateType>();

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

    QString url=qgetenv("HTTPAPIURL");
    this->httpAPIurl=url.size()!=0? url:HTTPAPIURL;
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
        emit playerStateChange();
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
    emit playerStateChange();
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
        emit playerStateChange();
        return json["roomId"].toString();
    }

    else
    {
        return nullptr;
    }
}

RoomList HttpAPIClient::listRooms(int page, int perPage)
{
    RoomList returnme;
    QJsonObject request;
    request.insert("page",page);
    request.insert("perPage",perPage);

    QJsonObject json = postRequest("/listRooms",request);
    QList<RoomInfo> results;
    foreach(QJsonValue j, json["results"].toArray())
    {
        QJsonObject o = j.toObject();
        RoomInfo r;
        r.roomName=o["roomname"].toString();
        r.roomId=(quint64)o["roomId"].toString().toLongLong();
        r.owner=o["owner"].toString();
        r.description=o["description"].toString();
        r.password=o["password"].toBool();

        results.append(r);
    }

    returnme.results=results;
    returnme.more=json["more"].toBool();

    return returnme;
}

RoomConnectionInfo HttpAPIClient::getCurrentRoom()
{
    QJsonObject json = getRequest("/getCurrentRoom");

    RoomConnectionInfo r;
    r.roomIp=json["roomIp"].toString();
    r.roomPort=json["roomPort"].toInt();
    r.roomId=json["roomId"].toInt();
    r.secretId=json["secretId"].toString().toUInt();
    r.owner=json["owner"].toBool();

    return r;
}

void HttpAPIClient::joinRoom(room_id_t roomId,QString password)
{
    QJsonObject request;
    request.insert("roomId",QString::number(roomId));
    if(password!=nullptr) request.insert("password",password);
    QJsonObject json=postRequest("/joinRoom",request);

    if(json["status"].toString()=="success") emit playerStateChange();
}

void HttpAPIClient::leaveRoom()
{
    QJsonObject request;
    QJsonObject json=postRequest("/leaveRoom",request);
    if(json["status"].toString()=="success") emit playerStateChange();
}

void HttpAPIClient::refreshPlayerState()
{
    emit playerStateChange();
}
#else   //server
HttpAPIClient::HttpAPIClient()
{
    this->token=qgetenv("TOKEN");

    QString url=qgetenv("HTTPAPIURL");
    this->httpAPIurl=url.size()!=0? url:HTTPAPIURL;
}

quint32 HttpAPIClient::getClientId(quint32 secretId, room_id_t roomId)
{
    QJsonObject requestParams;
    requestParams.insert("secretId",QString::number(secretId));
    requestParams.insert("roomId",roomId);
    QJsonObject json = postRequest("/getClientId",requestParams);

    if(json["status"].toString()=="success")
        return json["clientId"].toString().toUInt();
    else
        return 0;
}

void HttpAPIClient::closeRoom(room_id_t roomId,StopReason reason)
{
    QJsonObject requestParams;
    requestParams.insert("reason",MetaStopReason.valueToKey(reason));
    requestParams.insert("roomId",roomId);
    QJsonObject json = postRequest("/closeRoom",requestParams);
}

void HttpAPIClient::refreshRooms(QList<room_id_t> rooms, QList<RoomUpdate> *updated)
{
    QJsonObject requestParams;
    QJsonArray roomList;
    for(room_id_t id: rooms)
    {
        roomList.append(id);
    }
    requestParams.insert("known",roomList);
    QJsonObject json = postRequest("/refreshRooms",requestParams);

    for(QJsonValueRef c: json["updates"].toArray())
    {
        QJsonObject change=c.toObject();
        RoomUpdate update;
        update.type=(UpdateType)MetaUpdateType.keyToValue(change["type"].toString().toUtf8());
        update.roomId=change["roomId"].toInt();
        update.owner=change["owner"].toString().toUInt();
        updated->append(update);
    }
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

QString HttpAPIClient::getIp()
{
    QString returnme("ERROR");
    QString url("http://ifconfig.io/ip");
    QNetworkRequest request(url);
    QNetworkReply *reply=netman.get(request);
    while(!reply->isFinished()) qApp->processEvents();
    if (reply->error() != QNetworkReply::NoError) emit httpError(reply->error(),reply->errorString());
    else returnme=reply->readAll();
    reply->deleteLater();
    return returnme;
}
