#ifndef HTTPAPICLIENT_H
#define HTTPAPICLIENT_H

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaEnum>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QSettings>

#include "roomcommon.h"

#define ROOMINFOATTRCOUNT 5

enum RoomInfoAttrs {roomId,roomName,owner,description,password};

struct RoomInfo
{
    quint64 roomId=0;
    QString roomName="";
    QString owner="";
    QString description="";
    bool password=false;
};

struct RoomList
{
    QList<RoomInfo> results;
    bool more;
};

struct RoomConnectionInfo
{
    QString roomIp;
    quint16 roomPort;
    room_id_t roomId;
    quint32 secretId;
    bool owner=false;
};

class HttpAPIClient: public QObject
{
    Q_OBJECT

public:
    HttpAPIClient();

    enum StopReason:quint8 {TIMEOUT,CRASH,USER};
    Q_ENUM(StopReason);
    enum UpdateType:quint8 {ADD,REMOVE};
    Q_ENUM(UpdateType);
    static QMetaEnum MetaStopReason,MetaUpdateType;

    struct RoomUpdate
    {
        UpdateType type;
        room_id_t roomId;
        quint32 owner;
    };

public slots:   //common
    bool test();
    int getUserStatus();

#ifdef QT_GUI_LIB
public slots:   //client
    void signup(QString username,QString password);
    bool signin(QString username,QString password);
    void signout();
    QString getUsername();
    QString createRoom(QString name, QString description=nullptr, QString password=nullptr, bool isprivate=false);
    RoomList listRooms(int page,int perPage);
    RoomConnectionInfo getCurrentRoom();
    void joinRoom(quint64 roomId, QString password=nullptr);
    void leaveRoom();

public:
    void refreshPlayerState();

signals:
    void playerStateChange();
#else
public slots:   //server
    quint32 getClientId(quint32 secretId, room_id_t roomId);
    void closeRoom(room_id_t roomId, StopReason reason);
    void refreshRooms(QList<room_id_t> rooms, QList<RoomUpdate> *updated);
#endif

signals:
    void apiError(QString message);
    void httpError(int code,QString message);

private:
    QJsonObject getRequest(QString endpoint);
    QJsonObject postRequest(QString endpoint, QJsonObject requestParams);
    QString getIp();


    QSettings prefs;
    QNetworkAccessManager netman;
    QString token;
    QString httpAPIurl;
};

#endif // HTTPAPICLIENT_H
