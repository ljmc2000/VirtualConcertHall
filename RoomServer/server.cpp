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

    heartBeatTimeoutTimer.setInterval(1);
    connect(
        &heartBeatTimeoutTimer, SIGNAL(timeout()),
        this, SLOT(heartBeatTimeout())
    );
    heartBeatTimeoutTimer.start();
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
            data.append(INIT);
            data.append(index);
            QNetworkDatagram datagram(data, c.address, c.port);
            qSocket.writeDatagram(datagram);
        }

        break;

    case HEARTBEAT:
        {
            if(index != -1)
            {
                clientTimeouts[index]=0;
            }

            else
            {

                QNetworkDatagram disconnection(disconnectPayload,c.address,c.port);
                qSocket.writeDatagram(disconnection);
            }

            break;
        }

    case MIDI:
        sendToAll(data);
        break;
    }
}

void Server::heartBeatTimeout()
{
    for(int i=0; i<clients.size(); i++)
    {
        clientTimeouts[i]+=1;
        if(clientTimeouts[i]>SERVERHEARTBEATTIMEOUT)
        {
            qDebug() << "Client Disconnecting" << clients[i].address;
            disconnectClient(i);
        }
    }
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
    QByteArray data;
    data.append(INIT);
    data.append(clients.size());
    QNetworkDatagram datagram(data, c.address, c.port);
    qSocket.writeDatagram(datagram);
    clients.append(c);
    clientTimeouts.append(0);
}

void Server::disconnectClient(int index)
{
    QByteArray data(disconnectPayload);
    Client c = clients[index];
    QNetworkDatagram datagram(data, c.address, c.port);
    qSocket.writeDatagram(datagram);
    clients.removeAt(index);
    clientTimeouts.removeAt(index);
}
