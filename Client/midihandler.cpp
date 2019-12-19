#include "midihandler.h"

MidiHandler::MidiHandler(QUdpSocket *qSocket)
{
    QSettings prefs;

    midiin.openPort(prefs.value("midiPort").toInt());
    midiin.setClientName("VirtualConcertHallClient");
    midiin.setCallback(handleMidi, this);

    this->qSocket=qSocket;
}

MidiHandler::~MidiHandler()
{
    midiin.closePort();
}

void MidiHandler::handleMidi( double timeStamp, std::vector<unsigned char> *message, void *userData )
{
    MidiHandler* self = static_cast<MidiHandler*>(userData);
    QByteArray data((char*)message->data(),message->size());
    QNetworkDatagram datagram(data);

    for(unsigned int i=0; i<message->size(); i++)
    {
        std::cout << (int)message->at(i) << ":";
    }

    std::cout << "\n";

    self->qSocket->writeDatagram(datagram);
}
