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
    QByteArray data1;
    data1.setNum(self->timestamp);
    data.insert(0,MIDI);
    data.insert(1,self->clientId);
    data.insert(2,data1);
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
            QByteArray data1(data.constBegin()+2,data.size());
            clientId=data.at(1);
            timestamp=data1.toLongLong();
            reconnectClock.stop();
            midiin.setCallback(handleMidi, this);
            break;
        }

    case HEARTBEAT:
        {
            QByteArray data1(data.constBegin()+2,data.size());
            timestamp=data1.toLongLong();
            QNetworkDatagram rsvp(data);
            qSocket.writeDatagram(rsvp);
            break;
        }

    case MIDI:
        {
            QByteArray data1(data.constBegin()+2,data.size()-5);
            qint64 t = data1.toLongLong();
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
