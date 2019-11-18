#include "server.h"

Server::Server(int port)
{
    qSocket.bind(QHostAddress::Any,port);
    std::cout << "Listening on port " << port << "\n";

    connect(
        &qSocket, SIGNAL(readyRead()),
        this, SLOT(readPendingDatagrams())
    );



    midiout.openPort(0);
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
        std::vector<unsigned char> mididata;

        for(unsigned int i=0; i<data.size(); i++)
        {
            std::cout << (int)(unsigned char)data.at(i) << ":";
            mididata.push_back((unsigned char)data.at(i));
        }

        std::cout << "\n";

        midiout.sendMessage(&mididata);
    }
}
