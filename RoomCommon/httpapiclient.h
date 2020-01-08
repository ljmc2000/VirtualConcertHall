#ifndef HTTPAPICLIENT_H
#define HTTPAPICLIENT_H

#include <QObject>
#include <QSettings>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

struct RoomInfo
{
    QString roomId;
    QString roomName;
    QString owner;
    QString description;
};

struct RoomConnectionInfo
{
    QString roomIp;
    quint16 roomPort;
    quint32 secretId;
};

class HttpAPIClient: public QObject
{
    Q_OBJECT

public:
    HttpAPIClient();
    HttpAPIClient(QString token);

public slots:   //common
    bool test();

public slots:   //client
    void signup(QString username,QString password);
    bool signin(QString username,QString password);
    QString createRoom(QString name);
    QList<RoomInfo> listRooms();
    RoomConnectionInfo getCurrentRoom();
    void joinRoom(QString roomId);
    void leaveRoom();
    void closeRoom();

public slots:   //server
    quint32 getClientId(quint32 secretId);

signals:
    void apiError(QString message);
    void httpError(int code,QString message);

private:
    QJsonObject getRequest(QString endpoint);
    QJsonObject postRequest(QString endpoint, QJsonObject requestParams);


    QSettings prefs;
    QNetworkAccessManager netman;
    QString token;
};

#endif // HTTPAPICLIENT_H
