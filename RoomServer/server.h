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

class Server: public QObject
{
    Q_OBJECT;

public:
    Server(int port);
    ~Server();

private slots:
    void readPendingDatagrams();
    void heartBeat();
    void pruneClients();
    void finish();

private:
    QUdpSocket qSocket;
    HttpAPIClient hapicli;
    QTimer heartBeatTimer;
    QTimer pruneTimer;
    QTimer idleTimeoutTimer;
    QHash<quint32,Client> clients;
    quint32 owner;

    void sendToAll(QByteArray data);
    bool addClient(QNetworkDatagram joinRequest);
    void disableClient(quint32 secretId);
    void enableClient(QNetworkDatagram joinRequest);
};

#endif // SERVER_H
