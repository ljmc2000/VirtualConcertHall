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
        sendMidiToClients(data);
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

void Server::sendMidiToClients(QByteArray data)
{
    int i = 0;
    foreach(Client client, clients)
    {
        data.remove(0,1);
        data.insert(0,i);
        i+=1;

        QNetworkDatagram datagram(data,client.address,client.port);
        qSocket.writeDatagram(datagram);
    }
}
