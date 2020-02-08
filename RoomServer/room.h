#ifndef SERVER_H
#define SERVER_H

#include <QCoreApplication>
#include <QDateTime>
#include <QNetworkDatagram>
#include <QObject>
#include <QTimer>
#include <QUdpSocket>

#include "httpapiclient.h"
#include "roomcommon.h"

#define GETTIME() QDateTime::currentDateTime().toMSecsSinceEpoch()

using namespace RoomCommon;

struct Client
{
    QHostAddress address;
    quint16 port;
    quint32 clientId;
    InstrumentType instrument;
    quint64 instrumentArgs;
    qint64 lastMessage=GETTIME();
    bool awake=true;
};

class Room: public QObject
{
    Q_OBJECT;

public:
    Room(quint64 roomID,quint32 owner,quint16 port,HttpAPIClient *httpapiclient, QObject *parent);
    ~Room();

private slots:
    void readPendingDatagrams();
    void heartBeat();
    void pruneClients();
    void finish(HttpAPIClient::StopReason reason);

private:
    QUdpSocket qSocket;
    HttpAPIClient *hapicli;
    QTimer heartBeatTimer;
    QTimer pruneTimer;
    QTimer idleTimeoutTimer;
    QHash<quint32,Client> clients;
    quint32 owner;
    quint64 roomID;

    void sendToAll(QByteArray data);
    bool addClient(QNetworkDatagram joinRequest);
    void disableClient(quint32 secretId);
    void enableClient(QNetworkDatagram joinRequest);
};

#endif // SERVER_H
