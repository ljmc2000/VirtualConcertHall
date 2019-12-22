#include "midihandler.h"
#include "roomcommon.h"

using namespace RoomCommon;

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
    QByteArray data((char *)message->data(),message->size());
    data.insert(0,MIDI);
    QNetworkDatagram datagram(data);
    self->qSocket->writeDatagram(datagram);
}
