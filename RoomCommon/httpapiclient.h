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

class HttpAPIClient: public QObject
{
    Q_OBJECT

public:
    HttpAPIClient();
    HttpAPIClient(QString token);

public slots:
    bool test();
    void signup(QString username,QString password);
    bool signin(QString username,QString password);
    QString createRoom(QString name);
    QList<RoomInfo> listRooms();
    void closeRoom();

signals:
    void apiError(QString message);
    void httpError(int code,QString message);

private:
    QJsonObject getRequest(QString endpoint);
    QJsonObject postRequest(QString endpoint, QJsonObject requestParams);


    QSettings prefs;
    QNetworkAccessManager netman;
    QString token;

    const static QString httpAPIurl;
};

#endif // HTTPAPICLIENT_H
