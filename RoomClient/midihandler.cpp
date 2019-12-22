#include "midihandler.h"
#include "roomcommon.h"

using namespace RoomCommon;

MidiHandler::MidiHandler()
{
    QSettings prefs;

    int midiInPort = prefs.value("midiInPort").toInt();
    midiin.openPort(midiInPort<midiin.getPortCount() ? midiInPort:0);
    midiin.setClientName("VirtualConcertHallClient");
    midiin.setCallback(handleMidi, this);

    int midiOutPort = prefs.value("midiOutPort").toInt();
    midiout.openPort(midiOutPort<midiin.getPortCount() ? midiOutPort:0);

    qSocket.connectToHost(QHostAddress(prefs.value("serverHost").toString()),prefs.value("serverPort").toInt());
    connect(
                &qSocket, SIGNAL(readyRead()),
                this, SLOT(handleDataFromServer())
            );

    heartBeatClock.setInterval(HEARTBEATINTERVAL);
    connect(
                &heartBeatClock, SIGNAL(timeout()),
                this, SLOT(heartBeat())
            );
    heartBeatClock.start();
}

MidiHandler::~MidiHandler()
{
    midiin.closePort();
    midiout.closePort();
}

void MidiHandler::handleMidi( double timeStamp, std::vector<unsigned char> *message, void *userData )
{
    MidiHandler* self = static_cast<MidiHandler*>(userData);
    QByteArray data((char *)message->data(),message->size());
    data.insert(0,MIDI);
    QNetworkDatagram datagram(data);
    self->qSocket.writeDatagram(datagram);
}

void MidiHandler::handleDataFromServer()
{
    //deal with server stuff here
    QNetworkDatagram datagram = qSocket.receiveDatagram();
    QByteArray data = datagram.data();

    foreach(unsigned char c, data)
    {
        std::cout << (int) c << ":";
    }
    std::cout << "\n";

    midiout.sendMessage((unsigned char*)data.constData()+1,data.count()-1);
}

void MidiHandler::heartBeat()
{
    QByteArray data;
    data.push_front(HEARTBEAT);

    qSocket.writeDatagram(QNetworkDatagram(data));
}
