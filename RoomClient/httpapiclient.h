#ifndef HTTPAPICLIENT_H
#define HTTPAPICLIENT_H

#include <QObject>
#include <QSettings>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "roomcommon.h"

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

public slots:
    bool test();
    void signup(QString username,QString password);
    bool signin(QString username,QString password);
    QString createRoom(QString name);
    QList<RoomInfo> listRooms();
    void closeRoom();

signals:
    void apiError(QString message);

private:
    QNetworkRequest getRequest(QString endpoint);

    QSettings prefs;
    QNetworkAccessManager netman;
};

#endif // HTTPAPICLIENT_H
