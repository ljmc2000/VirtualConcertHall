#ifndef ROOMCOMMON_H
#define ROOMCOMMON_H

#define HEARTBEATINTERVAL 40        //how often to send heartbeat messages
#define SERVERHEARTBEATTIMEOUT 50   //maximum accepted latency for a packet
#define SERVERTIMEOUT 3000         //how long to wait since last packet before marking as dorment
#define MIDIMESSAGESIZE 3           //the size in bytes of a midi message
#define RECONNECTDELAY 3000         //time waited before attempting to reconnect to server
#define PRUNINGINTERVAL 5000        //how often to check for dead clients
#define SERVERTIMEUPDATEINTERVAL 1  //increment the time sent by the heartbeat packet every n seconds

namespace RoomCommon
{
    enum PacketType {CONNECT,           //sent to request connection
                     INIT,              //sent upon connection
                     HEARTBEAT,         //sent constantly to ensure continued connection
                     MIDI,              //a packet which contains midi data
                     DISABLE,ENABLE,    //notification of a client being enabled or disabled
                     DISCONNECT         //sent to inform a player they have been disconnected
                    };

    struct ConnectPacket
    {
        PacketType packetType=CONNECT;
        quint32 secretId;
    };

    struct InitPacket
    {
        PacketType packetType=INIT;
        quint32 clientId;
        qint64 timestamp;
    };

    struct HeartbeatPacket
    {
        PacketType packetType=HEARTBEAT;
        quint32 secretId=-1;
        qint64 timestamp;
    };

    struct MidiPacket
    {
        PacketType packetType=MIDI;
        quint32 clientId;
        qint64 timestamp;
        quint8 message[3] = {0,0,0};
    };

    struct DisablePacket
    {
        PacketType packetType=DISABLE;
        quint32 clientId;
    };

    struct EnablePacket
    {
        PacketType packetType=ENABLE;
        quint32 clientId;
    };

    struct DisconnectPacket
    {
        PacketType packetType=DISCONNECT;
    };
    static DisconnectPacket disconnectPacket;

    static QList<quint8> noisyMessages(  //messages that if delivered late would be distracting
    {
        144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159 //on message for all 16 channels
    });
}
#endif
