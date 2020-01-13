#ifndef HTTPAPICLIENT_H
#define HTTPAPICLIENT_H

#include <QObject>
#include <QSettings>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#define ROOMINFOATTRCOUNT 4

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
    bool owner=false;
};

class HttpAPIClient: public QObject
{
    Q_OBJECT

public:
    HttpAPIClient();

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
    QList<RoomInfo> listRooms(int page,int perPage);
    RoomConnectionInfo getCurrentRoom();
    void joinRoom(QString roomId);
    void leaveRoom();
    void closeRoom();

signals:
    void roomReady();
#else
public slots:   //server
    quint32 getClientId(quint32 secretId);
    void timeoutRoom();
#endif

signals:
    void more(bool more);
    void tokenChange();
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
