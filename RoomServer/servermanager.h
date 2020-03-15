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

    void addServer(room_id_t roomID, client_id_t owner), removeServer(room_id_t roomID);

public slots:
    void refreshServers();
    void handlePackets();

private:
    QUdpSocket qSocket;
    QHash<room_id_t,Room*> rooms;
    HttpAPIClient httpapicli;
    QList<HttpAPIClient::RoomUpdate> roomUpdates;
};

#endif // SERVERMANAGER_H
