#include "midihandler.h"

using namespace RoomCommon;

MidiHandler::MidiHandler(quint32 secretId, QString address, quint16 port, QObject *parent):
    QObject(parent)
{
    QSettings prefs;
    serverHost = QHostAddress(address);
    serverPort = port;

    unsigned int midiInPort = prefs.value("midiInPort").toUInt();
    midiin.openPort(midiInPort<midiin.getPortCount() ? midiInPort:0);
    midiin.setClientName("VirtualConcertHallClient");

    soundfont=prefs.value("soundfont").toString();
    audioDriver=prefs.value("audioDriver").toString();

    loadInstrumentConfig(&prefs);

    serverTimeIterator.setInterval(SERVERTIMEUPDATEINTERVAL);
    connect(
                &serverTimeIterator, SIGNAL(timeout()),
                this, SLOT(iterateServertime())
            );

    connect(
                &qSocket, SIGNAL(readyRead()),
                this, SLOT(handleDataFromServer())
            );

    reconnectClock.setInterval(RECONNECTDELAY);
    connect(
                &reconnectClock, SIGNAL(timeout()),
                this, SLOT(attemptConnect())
            );
    this->secretId=secretId;
    attemptConnect();
    reconnectClock.start();
}

MidiHandler::~MidiHandler()
{
    disconnectFromServer();

    for(quint32 clientId: midiout.keys())
    {
        delSynth(clientId);
    }

    reconnectClock.stop();
    midiin.closePort();
}

void MidiHandler::closeServer()
{
    CloseServerPacket closeServerPacket;
    closeServerPacket.secretId=secretId;
    QByteArray data((char *)&closeServerPacket, sizeof (DisconnectPacket));
    QNetworkDatagram datagram(data,serverHost,serverPort);
    qSocket.writeDatagram(datagram);
    disconnectFromServer();
}

void MidiHandler::disconnectFromServer()
{
    if(qSocket.isOpen())
    {
        DisconnectPacket disconnectPacket;
        disconnectPacket.secretId=secretId;
        QByteArray data((char *)&disconnectPacket, sizeof (DisconnectPacket));
        QNetworkDatagram datagram(data,serverHost,serverPort);
        qSocket.writeDatagram(datagram);
        qSocket.disconnectFromHost();
    }
}

void MidiHandler::loadInstrumentConfig(QSettings *prefs)
{
    insturmentType=(InstrumentType) prefs->value("instrumentType").toInt();
    quint8 *args=(quint8*) &instrumentArgs;

    switch (insturmentType)
    {
    case PIANO:
        args[0]=(quint8)prefs->value("minNote").toUInt();
        args[1]=(quint8)prefs->value("maxNote").toUInt();
    }
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
    while (qSocket.hasPendingDatagrams())
    {
        //deal with server stuff here
        QNetworkDatagram datagram = qSocket.receiveDatagram();
        QByteArray data = datagram.data();

        if(verifyPacketSize((PacketType) data.at(0), data.size())) switch(data.at(0))
        {
        case INIT:
            {
                InitPacket *initPacket=(InitPacket*) data.constData();
                clientId=initPacket->clientId;
                timestamp=initPacket->timestamp;
                reconnectClock.stop();
                serverTimeIterator.start();
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
                emit midiMessage(midiPacket->clientId,midiPacket->message);
                break;
            }

        case DISABLE:
            {
                DisablePacket *disablePacket=(DisablePacket*) data.constData();
                qDebug() << "player" << disablePacket->clientId << "has gone dormant";
                addSynth(disablePacket->clientId);
                emit playerLeave(disablePacket->clientId);
                break;
            }

        case ENABLE:    //TODO add behaviour for un greying out players who have gone dormant
            {
                EnablePacket *enablePacket=(EnablePacket*) data.constData();
                qDebug() << "player" << enablePacket->clientId << "has awoken";
                addSynth(enablePacket->clientId);
                emit playerJoin(enablePacket->clientId, enablePacket->instrument, enablePacket->instrumentArgs);
                break;
            }

        case DISCONNECT:
            {
                deleteLater();
                break;
            }
        }

        else
        {
            qDebug() << "WARNING: Improperly sized packet";
        }
    }
}

void MidiHandler::handleMidiFromServer(quint32 clientId, qint64 timestamp, quint8 *midiMessage)
{
    fluid_synth_t* synth=midiout[clientId];
    quint8 channel=midiMessage[0]%16;

    if(synth != nullptr) switch(midiMessage[0]>>4)
    {
    case 0b1000:    //note off event
        fluid_synth_noteoff(synth,channel,midiMessage[1]);
        break;

    case 0b1001:    //note on event
        if(midiMessage[2]==0) {
            fluid_synth_noteoff(synth,channel,midiMessage[1]);
        } else if(timestamp+SERVERHEARTBEATTIMEOUT<this->timestamp) {
            qDebug() << "Midi packet dropped from" << clientId;
        } else {
            fluid_synth_noteon(synth,channel,midiMessage[1],midiMessage[2]);
        }
        break;

    case 0b1010:    //Polyphonic Key Pressure
        fluid_synth_key_pressure(synth,channel,midiMessage[1],midiMessage[2]);
        break;

    case 0b1011:    //Control Change
        fluid_synth_cc(synth,channel,midiMessage[1],midiMessage[2]);
        break;

    case 0b1100:    //Program Change
        fluid_synth_program_change(synth,channel,midiMessage[1]);
        break;

    case 0b1101:    //Channel Pressure
        fluid_synth_channel_pressure(synth,channel,midiMessage[1]);
        break;

    case 0b1110:    //Pitch Bend Change
        int value=midiMessage[1]+(midiMessage[2]<<7);
        fluid_synth_pitch_bend(synth,channel,value);
        break;
    }

    else
    {
        qDebug() << "synthesiser error";
    }

    QString m;
    for(unsigned int i=0; i<MIDIMESSAGESIZE; i++) m.append(QString::number(midiMessage[i])+":");
    qDebug() << clientId << m << this->timestamp-timestamp<<"ms";
}

void MidiHandler::attemptConnect()
{
    if(reconnectAttempts!=0)
    {
        reconnectAttempts--;
        qDebug() << "attempting connection to server";
        ConnectPacket connectPacket;
        connectPacket.secretId=secretId;
        connectPacket.instrument=insturmentType;
        connectPacket.instrumentArgs=instrumentArgs;

        QByteArray data((char*)&connectPacket,sizeof(ConnectPacket));
        qSocket.disconnectFromHost();
        qSocket.connectToHost(serverHost,serverPort);
        qSocket.writeDatagram(QNetworkDatagram(data,serverHost,serverPort));
    }
    else
    {
        qDebug() << "Exiting after " << MAXCONNECTATTEMPTS << " tries";
        deleteLater();
    }
}

void MidiHandler::iterateServertime()
{
    timestamp+=SERVERTIMEUPDATEINTERVAL;
}

void MidiHandler::addSynth(quint32 clientId)
{
    if(!midiout.contains(clientId))
    {
        fluid_settings_t* fluidSettings=new_fluid_settings();
        fluid_settings_setstr(fluidSettings,"audio.driver",audioDriver.toUtf8().constData());
        midiout[clientId]=new_fluid_synth(fluidSettings);
        fluid_synth_sfload(midiout[clientId],soundfont.toUtf8().constData(),true);
        soundout[clientId]=new_fluid_audio_driver(fluidSettings,midiout[clientId]);
    }
}

void MidiHandler::delSynth(quint32 clientId)
{
    fluid_settings_t* fluidSettings=fluid_synth_get_settings(midiout[clientId]);
    delete_fluid_audio_driver(soundout[clientId]);
    delete_fluid_settings(fluidSettings);
    delete_fluid_synth(midiout[clientId]);

    soundout.remove(clientId);
    midiout.remove(clientId);
}
