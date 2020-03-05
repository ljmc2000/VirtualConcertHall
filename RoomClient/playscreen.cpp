#include "playscreen.h"
#include "ui_playscreen.h"

#include <QMessageBox>
#include <QHostInfo>

#include "roomcommon.h"

using namespace RoomCommon;

PlayScreen::PlayScreen(RoomConnectionInfo r,HttpAPIClient *httpApiClient,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayScreen)
{
    ui->setupUi(this);

    this->httpApiClient=httpApiClient;

    this->owner=r.owner;
    this->serverHost=QHostInfo::fromName(r.roomIp).addresses().first();
    this->serverPort=r.roomPort;
    this->roomId=r.roomId;

    this->instrumentType=(InstrumentType)prefs.value("instrumentType").toInt();
    this->instrumentArgs=ui->midiout->getInstrumentArgs(&prefs,instrumentType);

    unsigned int midiInPort = prefs.value("midiInPort").toUInt();
    midiin.openPort(midiInPort<midiin.getPortCount() ? midiInPort:0);
    midiin.setClientName("VirtualConcertHallClient");
    midiin.setCallback(&handleMidiIn,this);

    connect(ui->exitButton, SIGNAL(clicked()),
            this, SLOT(askQuit()));

    connect(this, &PlayScreen::pingChange,
            [=](){ui->pingLabel->setText(QString::number(this->pingOffset)+"ms\n"+QString::number(ui->midiout->maxLatency)+"ms");});

    connect(ui->latencySlider, &QSlider::valueChanged,
            [=](){ui->midiout->maxLatency=ui->latencySlider->value();});

    pingOffsetSyncClock.setInterval(PINGPACKETINTERVAL);
    connect(
                &pingOffsetSyncClock, SIGNAL(timeout()),
                this, SLOT(ping())
            );
    pingOffsetSyncClock.start();

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
                reconnectClock.stop();
                break;
            }

        case HEARTBEAT:
            {
                HeartbeatPacket *heartbeatPacket=(HeartbeatPacket*) data.constData();
                heartbeatPacket->secretId=secretId;
                sendPacket(data.data(),HEARTBEAT);
                break;
            }

        case MIDI:
            {
                MidiPacket *midiPacket=(MidiPacket*) data.constData();
                int offset=GETTIME()-midiPacket->timestamp;
                ui->midiout->handleMidi(midiPacket->clientId,midiPacket->message,offset-timeOffset);
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

        case CHECKTIME:
            {
                CheckTimePacket *checkTimePacket=(CheckTimePacket*) data.constData();
                pingOffset=(GETTIME() - checkTimePacket->clientTime);
                timeOffset=GETTIME()-(pingOffset/2)-checkTimePacket->serverTime;
                emit pingChange(pingOffset);

                break;
            }

        case WHOOPSIE:
            {
                WhoopsiePacket *errorPacket=(WhoopsiePacket*) data.constData();
                QString errorMessage;
                switch(errorPacket->reason)
                {
                    case WRONGVERSION:
                        errorMessage="The server is running a different version of the communications protocol. Try updating your client.";
                        break;
                    case WRONGSIZEPACKET:
                        errorMessage="A malformed packet was sent to the server";
                        break;
                    case PLAYERNOTFOUND:
                        errorMessage="An attempt was made to join a server to which the user did not belong";
                }

                QMessageBox::warning(this,"An error has occured",errorMessage);
                quitPlaying();
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
    midiPacket.timestamp=GETTIME()-self->timeOffset;
    for(int i=0; i<message->size(); i++)midiPacket.message[i]=message->at(i);

    self->sendPacket((char*)&midiPacket,MIDI);
}

void PlayScreen::sendPacket(char *packet, PacketType packetType)
{
    QByteArray data(packet, packetSize[packetType]);
    int s = sizeof(room_id_t);
    data.append(QByteArray((char*)&roomId, s), s);
    QNetworkDatagram datagram(data,serverHost,serverPort);
    qSocket.writeDatagram(datagram);
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

void PlayScreen::ping()
{
    CheckTimePacket packet;
    packet.clientTime=GETTIME();
    QByteArray data((char*)&packet, packetSize[CHECKTIME]);
    qSocket.writeDatagram(data,serverHost,serverPort);
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
    sendPacket((char*)&closeServerPacket,CLOSESERVER);
    disconnectFromServer();
}

void PlayScreen::disconnectFromServer()
{
    if(qSocket.isOpen())
    {
        DisconnectPacket disconnectPacket;
        disconnectPacket.secretId=secretId;
        sendPacket((char *)&disconnectPacket, DISCONNECT);
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

        qSocket.disconnectFromHost();
        qSocket.connectToHost(serverHost,serverPort);
        sendPacket((char*)&connectPacket,CONNECT);
    }
    else
    {
        qDebug() << "Exiting after " << MAXCONNECTATTEMPTS << " tries";
        deleteLater();
    }
}
