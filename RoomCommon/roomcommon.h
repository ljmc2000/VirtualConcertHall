#ifndef ROOMCOMMON_H
#define ROOMCOMMON_H

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

namespace RoomCommon
{
    Q_NAMESPACE

    enum PacketType {CONNECT,           //sent to request connection
                     INIT,              //sent upon connection
                     HEARTBEAT,         //sent constantly to ensure continued connection
                     MIDI,              //a packet which contains midi data
                     DISABLE,ENABLE,    //notification of a client being enabled or disabled
                     DISCONNECT,        //sent to inform a player they have been disconnected
                     CLOSESERVER,       //can be sent by the owner to close the server
                    };

    enum InstrumentType
    {
        PIANO,
        GUITAR,

    }; Q_ENUM_NS(InstrumentType);

    struct ConnectPacket
    {
        PacketType packetType=CONNECT;
        quint32 secretId;
        InstrumentType instrument;
        quint64 instrumentArgs;
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
        quint64 instrumentArgs=0;      //use of fields will vary by instrament.
                                       //for example 0 and 1 will be min and max note for piano,
                                       //or each could represent a single guitar string's min note.
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
    };

    static bool verifyPacketSize(PacketType packetType,quint8 size) //a guard against buffer underflow attacks
    {
        if(!packetSize.contains(packetType))
            return false;
        else
            return packetSize[packetType] == size;
    }

    static QList<quint8> noisyMessages(  //messages that if delivered late would be distracting
    {
        144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159 //on message for all 16 channels
    });

    struct PianoArgs
    {
        quint8 minNote=0;
        quint8 maxNote=127;
    };

    struct GuitarArgs
    {
        quint8 lowestNote=40;   //lowest not of the lowest string
        quint8 fretCount=18;
        quint8 gaps1=0x55;      //the note gap from the first to second and second to third strings
        quint8 gaps2=0x54;      //the note gap from the third to forth and forth to fifth strings
        quint8 gaps3=0x50;      //the note gap from the fifth to sixth and sixth to seventh strings
        quint8 gaps4=0x00;      //etc
        quint8 gaps5=0x00;
        quint8 gaps6=0x00;
    };

    enum GuitarTuning
    {
        EADGBE,
        DADGAD,
    };
    Q_ENUM_NS(GuitarTuning);
}
#endif
