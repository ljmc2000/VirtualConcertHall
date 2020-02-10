#include "room.h"
#include "servermanager.h"

#define SENDWHOOPSIEPACKET(REASON,DATAGRAM) WhoopsiePacket whoopsiePacket;\
whoopsiePacket.reason=REASON;\
QByteArray data((char*)&whoopsiePacket,sizeof (WhoopsiePacket));\
QNetworkDatagram datagram1(data,DATAGRAM.senderAddress(),DATAGRAM.senderPort());\
qSocket->writeDatagram(datagram1);

using namespace RoomCommon;

Room::Room(room_id_t roomID, quint32 owner, QUdpSocket *qSocket, HttpAPIClient *httpapicli,QObject *parent): QObject(parent)
{
    this->roomID=roomID;
    this->hapicli=httpapicli;
    this->qSocket=qSocket;

    heartBeatTimer.setInterval(HEARTBEATINTERVAL);
    connect( &heartBeatTimer, SIGNAL(timeout()),
             this, SLOT(heartBeat()));
    heartBeatTimer.start();

    pruneTimer.setInterval(PRUNINGINTERVAL);
    connect(
        &pruneTimer, SIGNAL(timeout()),
        this, SLOT(pruneClients())
    );
    pruneTimer.start();

    idleTimeoutTimer.setSingleShot(true);
    idleTimeoutTimer.setInterval(IDLETIMEOUT);
    connect(&idleTimeoutTimer, &QTimer::timeout,[=](){
        finish(HttpAPIClient::TIMEOUT);
    });
    idleTimeoutTimer.start();

    this->owner=owner;
}

Room::~Room()
{

}

void Room::handlePacket(const QNetworkDatagram &datagram)
{
    idleTimeoutTimer.setInterval(IDLETIMEOUT);
    PacketType packetType = (PacketType) datagram.data().at(0);
    QByteArray data(datagram.data(),packetSize[packetType]);

    if(verifyPacketSize(packetType,datagram.data().size()-sizeof(room_id_t))) switch(data.at(0))
    {
    case CONNECT:
        {
            ConnectPacket *connectPacket=(ConnectPacket*) data.constData();
            if(connectPacket->version!=VERSION){
                SENDWHOOPSIEPACKET(WRONGVERSION,datagram);
            } else if(!clients.keys().contains(connectPacket->secretId)) {
                if(addClient(datagram))
                        enableClient(datagram);
            } else {
                enableClient(datagram);
            }
            break;
        }

    case DISCONNECT:
        {
            DisconnectPacket *disconnectPacket=(DisconnectPacket*) data.constData();
            disableClient(disconnectPacket->secretId);
            break;
        }

    case CLOSESERVER:
        {
            finish(HttpAPIClient::USER);
            break;
        }

    case HEARTBEAT:
        {
            HeartbeatPacket *heartbeatPacket=(HeartbeatPacket*) data.constData();
            clients[heartbeatPacket->secretId].lastMessage=heartbeatPacket->timestamp;
            break;
        }

    case MIDI:
        {
            MidiPacket *midiPacket=(MidiPacket*) data.constData();
            midiPacket->clientId=clients[midiPacket->clientId].clientId;
            sendToAll(data);
            break;
        }

    case PING:
    {
        PingPacket pingPacket;
        QByteArray data((char*)&pingPacket, sizeof(PingPacket));
        QNetworkDatagram datagram1(data,datagram.senderAddress(),datagram.senderPort());
        qSocket->writeDatagram(datagram1);
        break;
    }

    default:
        qDebug() << "unimplemented packet type recieved";
    }

    else
    {
        qDebug() << "WARNING: Improperly sized packet of type" << packetType << "and size" << data.size();
        SENDWHOOPSIEPACKET(WRONGSIZEPACKET,datagram);
    }

}

void Room::heartBeat()
{
    foreach(Client c,clients) if(c.awake)
    {
        HeartbeatPacket heartbeatPacket;
        heartbeatPacket.timestamp=GETTIME();

        QByteArray data((char*)&heartbeatPacket,sizeof(HeartbeatPacket));
        QNetworkDatagram datagram(data,c.address,c.port);
        qSocket->writeDatagram(datagram);
    }
}

void Room::pruneClients()
{
    for(quint32 secretId: clients.keys()) if(clients[secretId].awake)
    {
        if(clients[secretId].lastMessage+SERVERTIMEOUT<GETTIME())
        {
            disableClient(secretId);
        }
    }
}

void Room::finish(HttpAPIClient::StopReason reason)
{
    qDebug() << "Room is shutting down due to" << HttpAPIClient::MetaStopReason.valueToKey(reason);

    DisconnectPacket disconnectPacket;
    QByteArray data((char *)&disconnectPacket,sizeof (DisconnectPacket));
    sendToAll(data);

    hapicli->closeRoom(roomID,reason);
    ((ServerManager*)parent())->removeServer(roomID);
}

void Room::sendToAll(const QByteArray &data)
{
    foreach(Client c, clients) if(c.awake)
    {
        QNetworkDatagram datagram(data, c.address, c.port);
        qSocket->writeDatagram(datagram);
    }
}

bool Room::addClient(const QNetworkDatagram &joinRequest)
{
    QByteArray joinRequestData = joinRequest.data();
    ConnectPacket *connectPacket=(ConnectPacket*) joinRequestData.constData();
    quint32 clientId=hapicli->getClientId(connectPacket->secretId,roomID);

    if(clientId!=0)
    {
        Client c;
        c.clientId=clientId;
        clients[connectPacket->secretId]=c;
        qDebug() << clientId << "joined the Room";
        return true;
    }

    else
    {
        qDebug() << "A connection attempt was made by a client with an invalid clientId";
        SENDWHOOPSIEPACKET(PLAYERNOTFOUND,joinRequest);
        return false;
    }
}

void Room::disableClient(quint32 secretId)
{
    DisablePacket disablePacket;
    Client c = clients.value(secretId);
    c.awake=false;
    clients[secretId]=c;
    disablePacket.clientId=c.clientId;
    QByteArray data((char*)&disablePacket,sizeof (DisablePacket));
    sendToAll(data);
    qDebug() << "Client Disabled" << c.address << c.port;
}

void Room::enableClient(const QNetworkDatagram &joinRequest)
{
    QByteArray joinRequestData = joinRequest.data();
    ConnectPacket *connectPacket=(ConnectPacket*) joinRequestData.constData();
    quint32 secretId=connectPacket->secretId;

    Client c = clients.value(secretId);
    c.address=joinRequest.senderAddress();
    c.port=joinRequest.senderPort();
    c.instrument=connectPacket->instrument;
    c.instrumentArgs=connectPacket->instrumentArgs;
    c.lastMessage=GETTIME();
    c.awake=true;
    clients[secretId]=c;

    InitPacket initPacket;
    initPacket.clientId=c.clientId;
    initPacket.timestamp=GETTIME();

    QByteArray data((char*)&initPacket,sizeof(InitPacket));
    QNetworkDatagram datagram(data, c.address, c.port);
    qSocket->writeDatagram(datagram);
    qDebug() << "Client Connecting" << c.address << c.port;

    for(Client c: clients) if(c.awake)
    {
        EnablePacket enablePacket;
        enablePacket.clientId=c.clientId;
        enablePacket.instrument=c.instrument;
        enablePacket.instrumentArgs=c.instrumentArgs;
        QByteArray data((char*)&enablePacket,sizeof(EnablePacket));
        QNetworkDatagram datagram(data, joinRequest.senderAddress(), joinRequest.senderPort());
        qSocket->writeDatagram(datagram);
    }

    EnablePacket enablePacket;
    enablePacket.clientId=c.clientId;
    enablePacket.instrument=c.instrument;
    enablePacket.instrumentArgs=c.instrumentArgs;

    QByteArray data1((char*)&enablePacket,sizeof(EnablePacket));
    sendToAll(data1);
}
