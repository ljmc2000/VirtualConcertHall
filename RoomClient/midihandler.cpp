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

    reconnectClock.setInterval(RECONNECTDELAY);
    connect(
                &reconnectClock, SIGNAL(timeout()),
                this, SLOT(attemptConnect())
            );
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
    QByteArray data((char *)message->data(),message->size());
    data.insert(0,MIDI);
    data.insert(1,self->clientId);
    data.insert(2,(char*)&self->timestamp,sizeof(qint64));
    QNetworkDatagram datagram(data);
    self->qSocket.writeDatagram(datagram);

    return;
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
            clientId=data.at(1);
            timestamp=*(char*)(data.constData()+2);
            reconnectClock.stop();
            midiin.setCallback(handleMidi, this);
            break;
        }

    case HEARTBEAT:
        {
            timestamp=*(qint64*)(data.constBegin()+1);
            QNetworkDatagram rsvp(data);
            qSocket.writeDatagram(rsvp);
            break;
        }

    case MIDI:
        {
            qint64 t = *(qint64*)(data.constBegin()+2);
            if(t<timestamp+HEARTBEATINTERVAL) midiout.sendMessage((unsigned char*)data.constEnd()-MIDIMESSAGESIZE,MIDIMESSAGESIZE);
            break;
        }

    case UPDATENUMBER:
        clientId=data.at(1);
        break;

    case DISCONNECT:
        clientId=-1;
        reconnectClock.start();
        midiin.cancelCallback();
        break;
    }
}

void MidiHandler::attemptConnect()
{
    qDebug() << "attempting connection to server";

    QByteArray data;
    data.push_front(INIT);

    qSocket.writeDatagram(QNetworkDatagram(data));
}
