#ifndef ROOMCOMMON_H
#define ROOMCOMMON_H

#define VERSION 3   //increment every time the api changes in a breaking way

#include <QMetaEnum>

#define HEARTBEATINTERVAL 40        //how often to send heartbeat messages
#define SERVERHEARTBEATTIMEOUT 50   //maximum accepted latency for a packet
#define SERVERTIMEOUT 3000          //how long to wait since last packet before marking as dorment
#define MIDIMESSAGESIZE 3           //the size in bytes of a midi message
#define RECONNECTDELAY 3000         //time waited before attempting to reconnect to server
#define PRUNINGINTERVAL 5000        //how often to check for dead clients
#define SERVERTIMEUPDATEINTERVAL 1  //increment the time sent by the heartbeat packet every n seconds
#define IDLETIMEOUT 30000           //how long to wait before the server auto shuts down
#define MAXCONNECTATTEMPTS 5        //attempt to connect to server no more than n times

typedef quint16 instrument_args_t;

namespace RoomCommon
{
    Q_NAMESPACE;

    enum PacketType: qint8
    {
        CONNECT,           //sent to request connection
        INIT,              //sent upon connection
        HEARTBEAT,         //sent constantly to ensure continued connection
        MIDI,              //a packet which contains midi data
        DISABLE,ENABLE,    //notification of a client being enabled or disabled
        DISCONNECT,        //sent to inform a player they have been disconnected
        CLOSESERVER,       //can be sent by the owner to close the server
        WHOOPSIE=-1,       //if something goes really wrong | apparantly windows.h defines an enum called error
        PING=-2,           //sent by the httpapi to check if the server is alive
    };

    enum WhoopsieType: quint8
    {
        WRONGVERSION,       //if the server and client have different versions of roomcommon
        WRONGSIZEPACKET,    //if the user somehow sends a packet with the wrong size for it's header
        PLAYERNOTFOUND,     //if the player trys to connect to a server but the server cannot identify them
    };

    enum InstrumentType: quint8
    {
        PIANO,
        GUITAR,

    }; Q_ENUM_NS(InstrumentType);

    struct ConnectPacket
    {
        PacketType packetType=CONNECT;
        quint16 version=VERSION;
        quint32 secretId;
        InstrumentType instrument;
        instrument_args_t instrumentArgs;
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
        InstrumentType instrument;
        instrument_args_t instrumentArgs=0;     //use of fields will vary by instrament.
                                                //for example 0 and 1 will be min and max note for piano,
                                                //or each could represent a single guitar string's min note.
                                                //see pianoargs for more info
    };

    struct DisconnectPacket
    {
        PacketType packetType=DISCONNECT;
        quint32 secretId;
    };

    struct CloseServerPacket
    {
        PacketType packetType=CLOSESERVER;
        quint32 secretId;
    };

    struct WhoopsiePacket
    {
        PacketType packetType=WHOOPSIE;
        WhoopsieType reason;
    };

    struct PingPacket
    {
        PacketType packetType=PING;
    };

    static QHash<PacketType,quint8> packetSize
    {
        {CONNECT, sizeof (ConnectPacket)},
        {INIT, sizeof (InitPacket)},
        {HEARTBEAT, sizeof (HeartbeatPacket)},
        {MIDI, sizeof (MidiPacket)},
        {DISABLE, sizeof (DisablePacket)},
        {ENABLE, sizeof (EnablePacket)},
        {DISCONNECT, sizeof (DisconnectPacket)},
        {CLOSESERVER, sizeof (CloseServerPacket)},
        {WHOOPSIE, sizeof (WhoopsiePacket)},
        {PING, sizeof (PingPacket)},
    };

    static bool verifyPacketSize(PacketType packetType,quint8 size) //a guard against buffer underflow attacks
    {
        if(!packetSize.contains(packetType))
            return false;
        else
            return packetSize[packetType] == size;
    }

    struct PianoArgs
    {
        quint8 minNote=0;
        quint8 maxNote=127;
    };

    enum GuitarTuning
    {
        STANDARD,
        DADGAD,
    };
    Q_ENUM_NS(GuitarTuning);

    struct GuitarArgs
    {
        GuitarTuning tuning=STANDARD;
    };
}
#endif
