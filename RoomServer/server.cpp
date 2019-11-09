#include "server.h"

Server::Server(int port)
{
    qSocket.bind(QHostAddress::Any,port);
    std::cout << "Listening on port " << port << "\n";

    connect(
        &qSocket, SIGNAL(readyRead()),
        this, SLOT(readPendingDatagrams())
    );
}

Server::~Server()
{

}

void Server::readPendingDatagrams()
{
    while(qSocket.hasPendingDatagrams())
    {
        QNetworkDatagram datagram = qSocket.receiveDatagram();
        QByteArray data = datagram.data();

        for(unsigned int i=0; i<data.size(); i++)
        {
            std::cout << (int)(unsigned char)data.at(i) << ":";
        }

        std::cout << "\n";
    }
}
