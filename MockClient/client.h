#ifndef CLIENT_H
#define CLIENT_H

#include "roomcommon.h"
#include "RtMidi.h"
#include "QUdpSocket"
#include "QTimer"

using namespace RoomCommon;

struct RoomConnectionInfo
{
    QString roomIp;
    quint16 roomPort;
    room_id_t roomId;
    client_id_t secretId;
    bool owner=false;
};

class Client: public QObject
{
    Q_OBJECT
public:
    Client(RoomConnectionInfo & r, QString & username, InstrumentType instrument, instrument_args_t instrumentArgs);

    void ping();
    void handleDataFromServer();
    void sendPacket(char *packet, PacketType packetType);
    static void handleMidiIn(double timeStamp, std::vector<unsigned char> *message, void *userData);

private:
    QUdpSocket qSocket;
    RtMidiIn midiin;
    QTimer pingtimer;

    QHostAddress serverHost;
    quint16 serverPort;
    client_id_t secretId;
    room_id_t roomId;
    short pingOffset,timeOffset;
};

#endif // CLIENT_H
