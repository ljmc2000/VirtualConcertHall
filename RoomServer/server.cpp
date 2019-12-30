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

    switch(data.at(0))
    {
    case CONNECT:
        {
            ConnectPacket *connectPacket=(ConnectPacket*) data.constData();
            if(!clients.keys().contains(connectPacket->secretId))
                addClient(datagram);
            else
                enableClient(datagram);
            break;
        }

    case HEARTBEAT:
        {
            HeartbeatPacket *heartbeatPacket=(HeartbeatPacket*) data.constData();
            clients[heartbeatPacket->secretId].lastMessage=heartbeatPacket->timestamp;
        }        
        break;

    case MIDI:
        {
            MidiPacket *midiPacket=(MidiPacket*) data.constData();
            midiPacket->clientId=clients[midiPacket->clientId].clientId;
            sendToAll(data);
            break;
        }
    }
}

void Server::heartBeat()
{
    foreach(Client c,clients)
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

quint8 Server::getNextClientId()
{
    return nextClientId++;
}

void Server::sendToAll(QByteArray data)
{
    qint64 timestamp=GETTIME();
    foreach(Client c, clients) if(timestamp<c.lastMessage+SERVERTIMEOUT)
    {
        QNetworkDatagram datagram(data, c.address, c.port);
        qSocket.writeDatagram(datagram);
    }
}

quint32 Server::addClient(QNetworkDatagram joinRequest)
{
    QByteArray joinRequestData = joinRequest.data();
    ConnectPacket *connectPacket=(ConnectPacket*) joinRequestData.constData();
    Client c;
    c.clientId=getNextClientId();
    c.address=joinRequest.senderAddress();
    c.port=joinRequest.senderPort();
    clients[connectPacket->secretId]=c;

    InitPacket initPacket;
    initPacket.clientId=c.clientId;
    initPacket.timestamp=GETTIME();

    QByteArray data((char*)&initPacket,sizeof(InitPacket));
    QNetworkDatagram datagram(data, c.address, c.port);
    qSocket.writeDatagram(datagram);
    qDebug() << "Client Connecting" << c.address << c.port;

    return c.clientId;
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
    c.lastMessage=GETTIME();
    c.awake=true;
    clients[secretId]=c;

    EnablePacket enablePacket;
    enablePacket.clientId=c.clientId;

    QByteArray data1((char*)&enablePacket,sizeof(EnablePacket));
    QNetworkDatagram datagram1(data1, c.address, c.port);
    qSocket.writeDatagram(datagram1);



    InitPacket initPacket;
    initPacket.timestamp = GETTIME();
    initPacket.clientId=c.clientId;

    QByteArray data2((char*)&initPacket,sizeof(InitPacket));
    QNetworkDatagram datagram2(data2, c.address, c.port);
    qSocket.writeDatagram(datagram2);
    qDebug() << "Client Awoken" << c.address << c.port;
}
