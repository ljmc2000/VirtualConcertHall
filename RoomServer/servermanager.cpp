#include "servermanager.h"

ServerManager::ServerManager()
{
    QString sport=qgetenv("MANAGEMENT_PORT");
    quint16 port=sport==""? 1998:sport.toUShort();

    qSocket.bind(QHostAddress::Any,port);
    connect(&qSocket, SIGNAL(readyRead()),
            this, SLOT(refreshServers()));
    qDebug() << "Listening on port" << port;

    connect(&httpapicli, &HttpAPIClient::httpError, [=](){
        qWarning() << "Failed to properly connect to the httpapi. Something is very wrong.";
    });

    httpapicli.setServerIp(port);
    refreshServers();
}

ServerManager::~ServerManager()
{
    for(Room *room: rooms)
    {
        room->deleteLater();
    }
}

void ServerManager::refreshServers()
{
    while(qSocket.hasPendingDatagrams())
    {
        QNetworkDatagram datagram = qSocket.receiveDatagram();
        QNetworkDatagram reply(datagram.data(),datagram.senderAddress(),datagram.senderPort());
        qSocket.writeDatagram(reply);
    }

    httpapicli.refreshRooms(rooms.keys(),&roomUpdates);

    for(HttpAPIClient::RoomUpdate update: roomUpdates)
    {
        switch (update.type)
        {
            case HttpAPIClient::ADD:
                addServer(update.roomId,update.owner);
                break;
            case HttpAPIClient::REMOVE:
                removeServer(update.roomId);
                break;
        }
    }

    roomUpdates.clear();
}

void ServerManager::addServer(quint64 roomID, quint32 owner)
{
    rooms.insert(roomID, new Room(roomID, owner, nextPort, &httpapicli, this));
    nextPort++;
}

void ServerManager::removeServer(quint64 roomID)
{
    rooms[roomID]->deleteLater();
    rooms.remove(roomID);
}
