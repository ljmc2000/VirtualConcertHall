#include "server.h"
#include "roomcommon.h"

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
}

Server::~Server()
{

}

void Server::readPendingDatagrams()
{
    QNetworkDatagram datagram = qSocket.receiveDatagram();
    QByteArray data = datagram.data();
    Client c = Client(datagram.senderAddress(),datagram.senderPort());

    switch(data.at(0))
    {
    case CONNECT:
        if(!clients.keys().contains(c))
            addClient(c);
        else
            enableClient(c);
        break;

    case HEARTBEAT:
        {
            HeartbeatPacket *heartbeatPacket=(HeartbeatPacket*) data.constData();
            clients[c].lastMessage=heartbeatPacket->timestamp;
        }        
        break;

    case MIDI:
        sendToAll(data);
        break;
    }
}

void Server::heartBeat()
{
    foreach(Client c,clients.keys())
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
    foreach(Client c, clients.keys()) if(clients[c].awake)
    {
        if(clients[c].lastMessage+SERVERTIMEOUT<GETTIME())
        {
            disableClient(c);
        }
    }
}

quint8 Server::getNextClientId()
{
    return ++nextClientId;
}

void Server::sendToAll(QByteArray data)
{
    qint64 timestamp=GETTIME();
    foreach(Client c, clients.keys()) if(timestamp<clients[c].lastMessage+SERVERTIMEOUT)
    {
        QNetworkDatagram datagram(data, c.address, c.port);
        qSocket.writeDatagram(datagram);
    }
}

void Server::addClient(Client c)
{
    InitPacket initPacket;
    initPacket.clientId=getNextClientId();
    initPacket.timestamp=GETTIME();

    QByteArray data((char*)&initPacket,sizeof(InitPacket));
    QNetworkDatagram datagram(data, c.address, c.port);
    qSocket.writeDatagram(datagram);
    clients[c].clientId=initPacket.clientId;
    qDebug() << "Client Connecting" << c.address << c.port;
}

void Server::disableClient(Client c)
{
    DisablePacket disablePacket;
    disablePacket.clientId=clients[c].clientId;
    clients[c].awake=false;
    QByteArray data((char*)&disablePacket,sizeof (DisablePacket));
    sendToAll(data);
    qDebug() << "Client Disabled" << c.address << c.port;
}

void Server::enableClient(Client c)
{
    InitPacket initPacket;
    initPacket.timestamp = GETTIME();
    initPacket.clientId=clients[c].clientId;

    QByteArray data((char*)&initPacket,sizeof(InitPacket));
    QNetworkDatagram datagram(data, c.address, c.port);
    qSocket.writeDatagram(datagram);
    qDebug() << "Client Awoken" << c.address << c.port;
}
