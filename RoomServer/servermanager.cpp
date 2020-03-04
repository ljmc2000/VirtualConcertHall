#include "servermanager.h"
#include "roomcommon.h"

using namespace RoomCommon;

ServerManager::ServerManager()
{
    QString sport=qgetenv("SERVER_PORT");
    quint16 port=sport==""? 1998:sport.toUShort();

    qSocket.bind(QHostAddress::Any,port);
    connect(&qSocket, SIGNAL(readyRead()),
            this, SLOT(handlePackets()));
    qDebug() << "Listening on port" << port;

    connect(&httpapicli, &HttpAPIClient::httpError, [=](){
        qWarning() << "Failed to properly connect to the httpapi. Something is very wrong.";
    });

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
    qDebug() << "Updating server list";

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

void ServerManager::handlePackets()
{
    while(qSocket.hasPendingDatagrams())
    {
        QNetworkDatagram datagram = qSocket.receiveDatagram();
        QByteArray data = datagram.data();
        PacketType packetType=(PacketType)data.at(0);

        switch (packetType)
        {
            case WHOOPSIE:
            {
                break;
            }
            case REFRESHSERVERS:
            {
                refreshServers();
                break;
            }
            default:
            {
                if(data.size()>sizeof (room_id_t))
                {
                    room_id_t *roomID=(room_id_t*)(
                                data.data()
                                +data.size()
                                -sizeof(room_id_t)
                            );
                    Room *room=rooms.value(*roomID);
                    if(room != nullptr)room->handlePacket(datagram);
                    else if(packetType==PING)
                    {
                        PingPacket p;
                        p.packetType=WHOOPSIE;
                        qSocket.writeDatagram((char*)&p, packetSize[PING], datagram.senderAddress(), datagram.senderPort());
                    }
                    else qDebug() << "A packet was sent with an invalid room number: " << *roomID;
                }
                break;
            }
        }
    }
}

void ServerManager::addServer(room_id_t roomID, quint32 owner)
{
    rooms.insert(roomID, new Room(roomID, owner, &qSocket, &httpapicli, this));
    qDebug() << "Room" << roomID << "has opened";
}

void ServerManager::removeServer(room_id_t roomID)
{
    rooms[roomID]->deleteLater();
    rooms.remove(roomID);
    qDebug() << "Room" << roomID << "closed";
}
