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

#define ROOMINFOATTRCOUNT 5

enum RoomInfoAttrs {roomId,roomName,owner,description,password};

struct RoomInfo
{
    QString roomId="";
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
    quint32 secretId;
    bool owner=false;
};

class HttpAPIClient: public QObject
{
    Q_OBJECT

public:
    HttpAPIClient();

    enum StopReason {TIMEOUT,CRASH,USER};
    Q_ENUM(StopReason);
    static QMetaEnum MetaStopReason;

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
    void joinRoom(QString roomId, QString password=nullptr);
    void leaveRoom();

public:
    void refreshPlayerState();

signals:
    void playerStateChange();
#else
public slots:   //server
    quint32 getClientId(quint32 secretId);
    void closeRoom(StopReason reason);
#endif

signals:
    void apiError(QString message);
    void httpError(int code,QString message);

private:
    QJsonObject getRequest(QString endpoint);
    QJsonObject postRequest(QString endpoint, QJsonObject requestParams);


    QSettings prefs;
    QNetworkAccessManager netman;
    QString token;
    QString httpAPIurl;
};

#endif // HTTPAPICLIENT_H
