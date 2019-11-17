#include "midihandler.h"

MidiHandler::MidiHandler(QUdpSocket *qSocket, std::string hostname, int port)
{
    midiin.openPort(0);
    midiin.setClientName("VirtualConcertHallClient");
    midiin.setCallback(handleMidi, this);

    this->qSocket=qSocket;
    this->server=QHostAddress(hostname.c_str());
    this->serverPort=port;
}

MidiHandler::~MidiHandler()
{
    midiin.closePort();
}

void MidiHandler::handleMidi( double timeStamp, std::vector<unsigned char> *message, void *userData )
{
    MidiHandler* self = static_cast<MidiHandler*>(userData);
    QByteArray data((char*)message->data(),message->size());
    QNetworkDatagram datagram(data, self->server, self->serverPort);

    for(unsigned int i=0; i<message->size(); i++)
    {
        std::cout << (int)message->at(i) << ":";
    }

    std::cout << "\n";

    self->qSocket->writeDatagram(datagram);
}

std::vector<std::string> MidiHandler::getPorts()
{
    std::vector<std::string> ports;

    for(unsigned int i=0; i<midiin.getPortCount(); i++)
    {
        ports.push_back(midiin.getPortName(i));
    }

    return ports;
}

void MidiHandler::changePort(int port)
{
    midiin.closePort();
    midiin.openPort(port);
}

void MidiHandler::setAddress(QString address)
{
    this->server = QHostAddress(address);
}
