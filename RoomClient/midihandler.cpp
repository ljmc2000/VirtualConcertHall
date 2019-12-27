#include "midihandler.h"
#include "roomcommon.h"

using namespace RoomCommon;

MidiHandler::MidiHandler()
{
    QSettings prefs;

    int midiInPort = prefs.value("midiInPort").toInt();
    midiin.openPort(midiInPort<midiin.getPortCount() ? midiInPort:0);
    midiin.setClientName("VirtualConcertHallClient");

    int midiOutPort = prefs.value("midiOutPort").toInt();
    midiout.openPort(midiOutPort<midiout.getPortCount() ? midiOutPort:0);
    midiout.setClientName("VirtualConcertHallClient");

    qSocket.connectToHost(QHostAddress(prefs.value("serverHost").toString()),prefs.value("serverPort").toInt());
    connect(
                &qSocket, SIGNAL(readyRead()),
                this, SLOT(handleDataFromServer())
            );

    heartBeatClock.setInterval(RECONNECTDELAY);
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
    data.insert(1,self->clientId);
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

    switch(data.at(0))
    {
    case INIT:
        clientId=data.at(1);
        heartBeatClock.setInterval(HEARTBEATINTERVAL);
        midiin.setCallback(handleMidi, this);
        break;

    case MIDI:
        midiout.sendMessage((unsigned char*)data.constEnd()-MIDIMESSAGESIZE,MIDIMESSAGESIZE);
        break;

    case UPDATENUMBER:
        clientId=data.at(1);
        break;

    case DISCONNECT:
        clientId=-1;
        heartBeatClock.setInterval(RECONNECTDELAY);
        midiin.cancelCallback();
        break;
    }
}

void MidiHandler::heartBeat()
{
    QByteArray data;
    data.push_front(clientId == -1 ? INIT:HEARTBEAT);

    qSocket.writeDatagram(QNetworkDatagram(data));
}
