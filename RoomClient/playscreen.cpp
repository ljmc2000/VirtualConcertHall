#include "playscreen.h"
#include "ui_playscreen.h"

#include <QMessageBox>

#include "roomcommon.h"

using namespace RoomCommon;

PlayScreen::PlayScreen(RoomConnectionInfo r,HttpAPIClient *httpApiClient,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayScreen)
{
    ui->setupUi(this);

    this->httpApiClient=httpApiClient;
    this->owner=r.owner;
    this->serverHost=r.roomIp;
    this->serverPort=r.roomPort;

    this->instrumentType=(InstrumentType)prefs.value("instrumentType").toInt();
    this->instrumentArgs=prefs.value("instrumentArgs").toString().toULongLong(nullptr,16);

    unsigned int midiInPort = prefs.value("midiInPort").toUInt();
    midiin.openPort(midiInPort<midiin.getPortCount() ? midiInPort:0);
    midiin.setClientName("VirtualConcertHallClient");
    midiin.setCallback(&handleMidiIn,this);

    connect(ui->exitButton, SIGNAL(clicked()),
            this, SLOT(askQuit()));

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
    this->secretId=r.secretId;
    attemptConnect();
    reconnectClock.start();
}

PlayScreen::~PlayScreen()
{
    reconnectClock.stop();
    disconnectFromServer();

    delete ui;
}

void PlayScreen::handleDataFromServer()
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
                ui->midiout->handleMidi(midiPacket->clientId,midiPacket->message,midiPacket->timestamp-timestamp);
                break;
            }

        case DISABLE:
            {
                DisablePacket *disablePacket=(DisablePacket*) data.constData();
                qDebug() << "player" << disablePacket->clientId << "has gone dormant";
                ui->midiout->delChannel(disablePacket->clientId);
                break;
            }

        case ENABLE:    //TODO add behaviour for un greying out players who have gone dormant
            {
                EnablePacket *enablePacket=(EnablePacket*) data.constData();
                qDebug() << "player" << enablePacket->clientId << "has awoken";
                ui->midiout->addChannel(enablePacket->clientId, enablePacket->instrument, enablePacket->instrumentArgs);
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

void PlayScreen::handleMidiIn( double timeStamp, std::vector<unsigned char> *message, void *userData )
{
    PlayScreen *self=(PlayScreen*)userData;

    MidiPacket midiPacket;
    midiPacket.clientId=self->secretId;
    midiPacket.timestamp=self->timestamp;
    for(int i=0; i<MIDIMESSAGESIZE; i++)midiPacket.message[i]=message->at(i);

    QByteArray data((char*)&midiPacket,sizeof(MidiPacket));
    QNetworkDatagram datagram(data,self->serverHost,self->serverPort);
    self->qSocket.writeDatagram(datagram);
}

void PlayScreen::askQuit()
{
    QMessageBox::StandardButton reply=QMessageBox::question(this, "Leave?", owner ?
                                                                "Are you sure you wish to close the room?":
                                                                "Are you sure you wish to leave the room?",
                                                            QMessageBox::Yes|QMessageBox::No);
    switch (reply)
    {
    case QMessageBox::Yes:
        quitPlaying();
        break;
    default:
        break;
    }
}

void PlayScreen::quitPlaying()
{
    if(!owner)
    {
        httpApiClient->leaveRoom();
    }
    else
    {
        closeServer();
    }

    qDebug() << "Disconnected from server";
    httpApiClient->refreshPlayerState();
    emit switchScreen(MAINMENU);
}

void PlayScreen::closeServer()
{
    CloseServerPacket closeServerPacket;
    closeServerPacket.secretId=secretId;
    QByteArray data((char *)&closeServerPacket, sizeof (DisconnectPacket));
    QNetworkDatagram datagram(data,serverHost,serverPort);
    qSocket.writeDatagram(datagram);
    disconnectFromServer();
}

void PlayScreen::disconnectFromServer()
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

void PlayScreen::attemptConnect()
{
    if(reconnectAttempts!=0)
    {
        reconnectAttempts--;
        qDebug() << "attempting connection to server";
        ConnectPacket connectPacket;
        connectPacket.secretId=secretId;
        connectPacket.instrument=instrumentType;
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

void PlayScreen::iterateServertime()
{
    timestamp+=SERVERTIMEUPDATEINTERVAL;
}
