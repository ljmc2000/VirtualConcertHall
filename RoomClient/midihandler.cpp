#include "midihandler.h"
#include "roomcommon.h"

using namespace RoomCommon;

MidiHandler::MidiHandler()
{
    QSettings prefs;
    serverHost = QHostAddress(prefs.value("serverHost").toString());
    serverPort = prefs.value("serverPort").toUInt();

    unsigned int midiInPort = prefs.value("midiInPort").toUInt();
    midiin.openPort(midiInPort<midiin.getPortCount() ? midiInPort:0);
    midiin.setClientName("VirtualConcertHallClient");

    unsigned int midiOutPort = prefs.value("midiOutPort").toUInt();
    midiout.openPort(midiOutPort<midiout.getPortCount() ? midiOutPort:0);
    midiout.setClientName("VirtualConcertHallClient");

    qSocket.connectToHost(serverHost,serverPort);
    connect(
                &qSocket, SIGNAL(readyRead()),
                this, SLOT(handleDataFromServer())
            );

    reconnectClock.setInterval(RECONNECTDELAY);
    connect(
                &reconnectClock, SIGNAL(timeout()),
                this, SLOT(attemptConnect())
            );
    attemptConnect();
    reconnectClock.start();
}

MidiHandler::~MidiHandler()
{
    midiin.closePort();
    midiout.closePort();
}

void MidiHandler::handleMidi( double timeStamp, std::vector<unsigned char> *message, void *userData )
{
    MidiHandler* self = static_cast<MidiHandler*>(userData);
    MidiPacket midiPacket;
    midiPacket.clientId=self->clientId;
    midiPacket.timestamp=self->timestamp;
    for(unsigned int i=0; i<message->size(); i++) midiPacket.message[i]=message->at(i);

    QByteArray data((char *)&midiPacket,sizeof(midiPacket));
    QNetworkDatagram datagram(data,self->serverHost,self->serverPort);
    self->qSocket.writeDatagram(datagram);
}

void MidiHandler::handleDataFromServer()
{
    //deal with server stuff here
    QNetworkDatagram datagram = qSocket.receiveDatagram();
    QByteArray data = datagram.data();

    switch(data.at(0))
    {
    case INIT:
        {
            InitPacket *initPacket=(InitPacket*) data.constData();
            clientId=initPacket->clientId;
            timestamp=initPacket->timestamp;
            reconnectClock.stop();
            midiin.setCallback(handleMidi, this);
            break;
        }

    case HEARTBEAT:
        {
            HeartbeatPacket *heartbeatPacket=(HeartbeatPacket*) data.constData();
            timestamp=heartbeatPacket->timestamp;
            QNetworkDatagram rsvp(data,serverHost,serverPort);
            qSocket.writeDatagram(rsvp);
            break;
        }

    case MIDI:
        {
            MidiPacket *midiPacket=(MidiPacket*) data.constData();
            if(midiPacket->timestamp<timestamp+HEARTBEATINTERVAL) midiout.sendMessage(midiPacket->message,MIDIMESSAGESIZE);
            break;
        }

    case DISABLE:
        {
            DisablePacket *disablePacket=(DisablePacket*) data.constData();
            qDebug() << "player" << disablePacket->clientId << "has gone dormant";
            break;
        }

    case ENABLE:    //TODO add behaviour for un greying out players who have gone dormant
        {
            EnablePacket *enablePacket=(EnablePacket*) data.constData();
            qDebug() << "player" << enablePacket->clientId << "has awoken";
            break;
        }

    case DISCONNECT:
        {
            clientId=-1;
            reconnectClock.start();
            midiin.cancelCallback();
            break;
        }
    }
}

void MidiHandler::attemptConnect()
{
    qDebug() << "attempting connection to server";
    QSettings prefs;

    QByteArray data((char*)&connectPacket,sizeof(ConnectPacket));
    qSocket.writeDatagram(QNetworkDatagram(data,serverHost,serverPort));
}
