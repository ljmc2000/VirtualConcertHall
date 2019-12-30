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
    secretId=prefs.value("secretId").toUInt();
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
    midiPacket.clientId=self->secretId;
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
            heartbeatPacket->secretId=secretId;
            QNetworkDatagram rsvp(data,serverHost,serverPort);
            qSocket.writeDatagram(rsvp);
            break;
        }

    case MIDI:
        {
            MidiPacket *midiPacket=(MidiPacket*) data.constData();
            handleMidiFromServer(midiPacket->clientId,midiPacket->timestamp,midiPacket->message);
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

void MidiHandler::handleMidiFromServer(quint32 clientId, qint64 timestamp, quint8 *midiMessage)
{
    if(timestamp+SERVERHEARTBEATTIMEOUT<this->timestamp)
    if(midiMessage[2]!=0)
    if(noisyMessages.contains(midiMessage[0]))
    {
        qDebug() << "Midi packet dropped from" << clientId;
        return;
    }
    midiout.sendMessage(midiMessage,MIDIMESSAGESIZE);

    QString m;
    for(unsigned int i=0; i<MIDIMESSAGESIZE; i++) m.append(QString::number(midiMessage[i])+":");
    qDebug() << clientId << m << this->timestamp-timestamp<<"ms";
}

void MidiHandler::attemptConnect()
{
    qDebug() << "attempting connection to server";
    ConnectPacket connectPacket;
    connectPacket.secretId=secretId;

    QByteArray data((char*)&connectPacket,sizeof(ConnectPacket));
    qSocket.writeDatagram(QNetworkDatagram(data,serverHost,serverPort));
}