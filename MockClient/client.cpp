#include "client.h"
#include <QDateTime>
#include <QNetworkDatagram>

using namespace RoomCommon;

Client::Client(RoomConnectionInfo & r, QString & username, InstrumentType instrument, instrument_args_t instrumentArgs):
    serverHost(r.roomIp)
{
    this->serverPort=r.roomPort;
    this->secretId=r.secretId;
    this->roomId=r.roomId;

    qSocket.connectToHost(serverHost,serverPort);
    midiin.setCallback(&handleMidiIn,this);
    midiin.openPort();
    midiin.setClientName("VirtualConcertHallMockClient");
    midiin.setPortName(username.toUtf8().constData());

    ConnectPacket connectPacket;
    connectPacket.secretId=secretId;
    connectPacket.instrument=instrument;
    connectPacket.instrumentArgs=instrumentArgs;
    qSocket.waitForConnected();
    sendPacket((char*)&connectPacket,CONNECT);

    pingtimer.setInterval(PINGPACKETINTERVAL);
    pingtimer.start();
    connect(&pingtimer, &QTimer::timeout,
            this, &Client::ping);
    connect(&qSocket, &QUdpSocket::readyRead,
            this, &Client::handleDataFromServer);

}

void Client::handleMidiIn( double timeStamp, std::vector<unsigned char> *message, void *userData )
{
    Client *self=(Client*)userData;

    MidiPacket midiPacket;
    midiPacket.clientId=self->secretId;
    midiPacket.timestamp=GETTIME()-self->timeOffset;
    for(int i=0; i<message->size(); i++)midiPacket.message[i]=message->at(i);

    self->sendPacket((char*)&midiPacket,MIDI);
}

void Client::sendPacket(char *packet, PacketType packetType)
{
    QByteArray data(packet, packetSize[packetType]);
    int s = sizeof(room_id_t);
    data.append(QByteArray((char*)&roomId, s), s);
    QNetworkDatagram datagram(data,serverHost,serverPort);
    qSocket.writeDatagram(datagram);
}

void Client::handleDataFromServer()
{
    while(qSocket.hasPendingDatagrams())
    {
        QNetworkDatagram datagram=qSocket.receiveDatagram();
        QByteArray data = datagram.data();

        switch (data.at(0))
        {
            case CHECKTIME:
                {
                    CheckTimePacket *checkTimePacket=(CheckTimePacket*) data.constData();
                    pingOffset=(GETTIME() - checkTimePacket->clientTime);
                    timeOffset=GETTIME()-(pingOffset/2)-checkTimePacket->serverTime;

                    break;
                }
            case HEARTBEAT:
                {
                    HeartbeatPacket *heartbeatPacket=(HeartbeatPacket*) data.constData();
                    heartbeatPacket->secretId=secretId;
                    sendPacket(data.data(),HEARTBEAT);
                    break;
                }

            default:
                break;
        }
    }
}

void Client::ping()
{
    CheckTimePacket packet;
    packet.clientTime=GETTIME();
    QByteArray data((char*)&packet, packetSize[CHECKTIME]);
    qSocket.writeDatagram(data,serverHost,serverPort);
}
