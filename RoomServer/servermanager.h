#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QObject>
#include "room.h"

class ServerManager : public QObject
{
    Q_OBJECT
public:
    explicit ServerManager();
    ~ServerManager();

    void addServer(quint64 roomID, quint32 owner), removeServer(quint64 roomID);

public slots:
    void refreshServers();

private:
    QUdpSocket qSocket;
    QHash<quint64,Room*> rooms;
    HttpAPIClient httpapicli;
    QList<HttpAPIClient::RoomUpdate> roomUpdates;

    quint16 nextPort=10000;
};

#endif // SERVERMANAGER_H
