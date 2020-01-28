#include "server.h"

#define SENDWHOOPSIEPACKET(REASON,DATAGRAM) WhoopsiePacket whoopsiePacket;\
whoopsiePacket.reason=REASON;\
QByteArray data((char*)&whoopsiePacket,sizeof (WhoopsiePacket));\
QNetworkDatagram datagram1(data,DATAGRAM.senderAddress(),DATAGRAM.senderPort());\
qSocket.writeDatagram(datagram1);

using namespace RoomCommon;

Server::Server(int port)
{
    qSocket.bind(QHostAddress::Any,port);
    qDebug() << "Listening on port" << port;

    connect(
        &qSocket, SIGNAL(readyRead()),
        this, SLOT(readPendingDatagrams())
    );

    heartBeatTimer.setInterval(HEARTBEATINTERVAL);
    connect(
        &heartBeatTimer, SIGNAL(timeout()),
        this, SLOT(heartBeat())
    );
    heartBeatTimer.start();

    pruneTimer.setInterval(PRUNINGINTERVAL);
    connect(
        &pruneTimer, SIGNAL(timeout()),
        this, SLOT(pruneClients())
    );
    pruneTimer.start();

    idleTimeoutTimer.setSingleShot(true);
    idleTimeoutTimer.setInterval(IDLETIMEOUT);
    connect(&idleTimeoutTimer, SIGNAL(timeout()),
            this, SLOT(finish()));
    idleTimeoutTimer.start();

    owner=qgetenv("OWNER").toUInt();
}

Server::~Server()
{

}

void Server::readPendingDatagrams()
{
    while (qSocket.hasPendingDatagrams())
    {
        idleTimeoutTimer.setInterval(IDLETIMEOUT);
        QNetworkDatagram datagram = qSocket.receiveDatagram();
        QByteArray data = datagram.data();

        if(verifyPacketSize((PacketType) data.at(0),data.size())) switch(data.at(0))
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
                finish();
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
        default:
            qDebug() << "unimplemented packet type recieved";
        }

        else
        {
            qDebug() << "WARNING: Improperly sized packet";
            SENDWHOOPSIEPACKET(WRONGSIZEPACKET,datagram);
        }
    }
}

void Server::heartBeat()
{
    foreach(Client c,clients) if(c.awake)
    {
        HeartbeatPacket heartbeatPacket;
        heartbeatPacket.timestamp=GETTIME();

        QByteArray data((char*)&heartbeatPacket,sizeof(HeartbeatPacket));
        QNetworkDatagram datagram(data,c.address,c.port);
        qSocket.writeDatagram(datagram);
    }
}

void Server::pruneClients()
{
    foreach(quint32 secretId, clients.keys()) if(clients[secretId].awake)
    {
        if(clients[secretId].lastMessage+SERVERTIMEOUT<GETTIME())
        {
            disableClient(secretId);
        }
    }
}

void Server::finish()
{
    qDebug() << "server is shutting down";

    DisconnectPacket disconnectPacket;
    QByteArray data((char *)&disconnectPacket,sizeof (DisconnectPacket));
    sendToAll(data);

    hapicli.timeoutRoom();
    QCoreApplication::quit();
}

void Server::sendToAll(QByteArray data)
{
    foreach(Client c, clients) if(c.awake)
    {
        QNetworkDatagram datagram(data, c.address, c.port);
        qSocket.writeDatagram(datagram);
    }
}

bool Server::addClient(QNetworkDatagram joinRequest)
{
    QByteArray joinRequestData = joinRequest.data();
    ConnectPacket *connectPacket=(ConnectPacket*) joinRequestData.constData();
    quint32 clientId=hapicli.getClientId(connectPacket->secretId);

    if(clientId!=0)
    {
        Client c;
        c.clientId=clientId;
        clients[connectPacket->secretId]=c;
        qDebug() << clientId << "joined the server";
        return true;
    }

    else
    {
        qDebug() << "A connection attempt was made by a client with an invalid clientId";
        SENDWHOOPSIEPACKET(PLAYERNOTFOUND,joinRequest);
        return false;
    }
}

void Server::disableClient(quint32 secretId)
{
    DisablePacket disablePacket;
    Client c = clients[secretId];
    c.awake=false;
    clients[secretId]=c;
    disablePacket.clientId=c.clientId;
    QByteArray data((char*)&disablePacket,sizeof (DisablePacket));
    sendToAll(data);
    qDebug() << "Client Disabled" << c.address << c.port;
}

void Server::enableClient(QNetworkDatagram joinRequest)
{
    QByteArray joinRequestData = joinRequest.data();
    ConnectPacket *connectPacket=(ConnectPacket*) joinRequestData.constData();
    quint32 secretId=connectPacket->secretId;

    Client c = clients[secretId];
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
    qSocket.writeDatagram(datagram);
    qDebug() << "Client Connecting" << c.address << c.port;

    for(Client c: clients) if(c.awake)
    {
        EnablePacket enablePacket;
        enablePacket.clientId=c.clientId;
        enablePacket.instrument=c.instrument;
        enablePacket.instrumentArgs=c.instrumentArgs;
        QByteArray data((char*)&enablePacket,sizeof(EnablePacket));
        QNetworkDatagram datagram(data, joinRequest.senderAddress(), joinRequest.senderPort());
        qSocket.writeDatagram(datagram);
    }

    EnablePacket enablePacket;
    enablePacket.clientId=c.clientId;
    enablePacket.instrument=c.instrument;
    enablePacket.instrumentArgs=c.instrumentArgs;

    QByteArray data1((char*)&enablePacket,sizeof(EnablePacket));
    sendToAll(data1);
}
