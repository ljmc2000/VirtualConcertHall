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
    Q_OBJECT

public:
    Room(room_id_t roomID,quint32 owner, QUdpSocket *qSocket, HttpAPIClient *httpapiclient, QObject *parent);
    ~Room();

    void handlePacket(const QNetworkDatagram &datagram);

private slots:
    void heartBeat();
    void pruneClients();
    void finish(HttpAPIClient::StopReason reason);

private:
    QUdpSocket *qSocket;
    HttpAPIClient *hapicli;
    QTimer heartBeatTimer;
    QTimer pruneTimer;
    QTimer idleTimeoutTimer;
    QHash<quint32,Client*> clients;
    quint32 owner;
    room_id_t roomID;

    void sendToAll(const QByteArray &data);
    bool addClient(const QNetworkDatagram &joinRequest);
    void disableClient(quint32 secretId);
    void enableClient(const QNetworkDatagram &joinRequest);
};

#endif // SERVER_H
