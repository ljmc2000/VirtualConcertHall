#ifndef SERVER_H
#define SERVER_H

#define GETTIME() QDateTime::currentDateTime().toMSecsSinceEpoch()

#include <httpapiclient.h>

#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QTimer>
#include <QDateTime>
#include <QCoreApplication>

struct Client
{
    QHostAddress address;
    quint16 port;
    quint32 clientId;
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
    quint32 addClient(QNetworkDatagram joinRequest);
    void disableClient(quint32 secretId);
    void enableClient(QNetworkDatagram joinRequest);
};

#endif // SERVER_H
