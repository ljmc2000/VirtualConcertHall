#include "server.h"
#include "roomcommon.h"
#define GETTIME() QDateTime::currentDateTime().toMSecsSinceEpoch()

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
    int index = clients.indexOf(c);

    switch(data.at(0))
    {
    case CONNECT:
        if(index == -1)
        {
            qDebug() << "Client Connecting" << c.address;
            addClient(c);
        }

        else
        {
            InitPacket initPacket;
            initPacket.timestamp = GETTIME();
            initPacket.clientId=index;
            data.append((char*)&initPacket,sizeof(InitPacket));

            QByteArray data;
            QNetworkDatagram datagram(data, c.address, c.port);
            qSocket.writeDatagram(datagram);
        }

        break;

    case HEARTBEAT:
        {
            HeartbeatPacket *heartbeatPacket=(HeartbeatPacket*) data.constData();
            lastMessage[index]=heartbeatPacket->timestamp;

            if(heartbeatPacket->timestamp+SERVERHEARTBEATTIMEOUT<GETTIME())
            {
                QByteArray data((char*)&disconnectPacket,sizeof(DisconnectPacket));
                QNetworkDatagram disconnection(data,c.address,c.port);
                qSocket.writeDatagram(disconnection);
            }
        }        
        break;

    case MIDI:
        sendToAll(data);
        break;
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
    bool pruned=false;

    for(int i=0; i<lastMessage.size(); i++)
    {
        if(lastMessage[i]+SERVERHEARTBEATTIMEOUT<GETTIME())
        {
            pruned=true;
            disconnectClient(i);
            i--;    //as the previously 1 ahead client is now at current value of i
        }
    }

    if(pruned) updateNumbers();
}

void Server::sendToAll(QByteArray data)
{
    foreach(Client client, clients)
    {
        QNetworkDatagram datagram(data, client.address, client.port);
        qSocket.writeDatagram(datagram);
    }
}

void Server::addClient(Client c)
{
    InitPacket initPacket;
    initPacket.clientId=clients.size();
    initPacket.timestamp=GETTIME();

    QByteArray data((char*)&initPacket,sizeof(InitPacket));
    QNetworkDatagram datagram(data, c.address, c.port);
    qSocket.writeDatagram(datagram);
    clients.append(c);
    lastMessage.append(GETTIME());
}

void Server::updateNumbers()
{
    for(quint8 i=0; i<clients.size(); i++)
    {
        UpdateNumberPacket updateNumberPacket;
        updateNumberPacket.clientId=i;

        QByteArray data((char*)&updateNumberPacket,sizeof(UpdateNumberPacket));
        QNetworkDatagram datagram(data, clients[i].address, clients[i].port);
        qSocket.writeDatagram(datagram);
    }
}

void Server::disconnectClient(int index)
{
    QByteArray data((char*)&disconnectPacket,sizeof (DisconnectPacket));
    Client c = clients[index];
    QNetworkDatagram datagram(data, c.address, c.port);
    qSocket.writeDatagram(datagram);
    clients.removeAt(index);
    lastMessage.removeAt(index);
    qDebug() << "Client Disconnecting" << c.address;
}
