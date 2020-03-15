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

using namespace RoomCommon;

struct Client
{
    QHostAddress address;
    quint16 port;
    client_id_t clientId;
    InstrumentType instrument;
    quint64 instrumentArgs;
    qint64 lastMessage=GETTIME();
    bool awake=true;
};

class Room: public QObject
{
    Q_OBJECT

public:
    Room(room_id_t roomID, client_id_t owner, QUdpSocket *qSocket, HttpAPIClient *httpapiclient, QObject *parent);
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
    QHash<client_id_t,Client*> clients;
    client_id_t owner;
    room_id_t roomID;

    void sendToAll(const QByteArray &data);
    bool addClient(const QNetworkDatagram &joinRequest);
    void disableClient(client_id_t secretId);
    void enableClient(const QNetworkDatagram &joinRequest);
};

#endif // SERVER_H
