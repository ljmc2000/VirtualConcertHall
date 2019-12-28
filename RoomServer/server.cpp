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
    case INIT:
        if(index == -1)
        {
            qDebug() << "Client Connecting" << c.address;
            addClient(c);
        }

        else
        {
            QByteArray data;
            qint64 t = GETTIME();
            data.append(INIT);
            data.append(index);
            data.append((char*)&t,sizeof(qint64));
            QNetworkDatagram datagram(data, c.address, c.port);
            qSocket.writeDatagram(datagram);
        }

        break;

    case HEARTBEAT:
        {
            qint64 timestamp = *(qint64*)(data.constBegin()+1);
            lastMessage[index]=timestamp;

            if(timestamp+SERVERHEARTBEATTIMEOUT<GETTIME())
            {
                QNetworkDatagram disconnection(disconnectPayload,c.address,c.port);
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
        QByteArray data;
        qint64 t=GETTIME();
        data.append(HEARTBEAT);
        data.append((char*)&t,sizeof(qint64));

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
    qint64 t = GETTIME();
    QByteArray data;
    data.append(INIT);
    data.append(clients.size());
    data.append((char*)&t,sizeof(qint64));
    QNetworkDatagram datagram(data, c.address, c.port);
    qSocket.writeDatagram(datagram);
    clients.append(c);
    lastMessage.append(GETTIME());
}

void Server::updateNumbers()
{
    for(int i=0; i<clients.size(); i++)
    {
        QByteArray data;
        data.append(UPDATENUMBER);
        data.append(i);

        QNetworkDatagram datagram(data, clients[i].address, clients[i].port);
        qSocket.writeDatagram(datagram);
    }
}

void Server::disconnectClient(int index)
{
    QByteArray data(disconnectPayload);
    Client c = clients[index];
    QNetworkDatagram datagram(data, c.address, c.port);
    qSocket.writeDatagram(datagram);
    clients.removeAt(index);
    lastMessage.removeAt(index);
    qDebug() << "Client Disconnecting" << c.address;
}
