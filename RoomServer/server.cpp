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

    switch(data.at(0))
    {
        case HEARTBEAT:
        {
            Client c = Client(datagram.senderAddress(),datagram.senderPort());
            int index = clients.indexOf(c);
            if(index != -1)
            {
                clientTimeouts[index]=0;
            }
            else
            {
                clients.append(c);
                clientTimeouts.append(0);
            }

            break;
        }

    case MIDI:
        sendMidiToClients(datagram);
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
            clients.removeAt(i);
            clientTimeouts.removeAt(i);
        }
    }
}

void Server::sendMidiToClients(QNetworkDatagram datagram)
{
    QByteArray data = datagram.data();
    Client c = Client(datagram.senderAddress(),datagram.senderPort());
    int index = clients.indexOf(c);
    data.remove(0,1);
    data.insert(0,index);

    for(int i=0; i<clients.size(); i++)
    {
        QNetworkDatagram datagram(data,clients[i].address,clients[i].port);
        qSocket.writeDatagram(datagram);
    }
}
