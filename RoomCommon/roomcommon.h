#ifndef ROOMCOMMON_H
#define ROOMCOMMON_H

#define VERSION 6   //increment every time the api changes in a breaking way

#include <QMetaEnum>

#define HEARTBEATINTERVAL 300       //how often to send heartbeat messages
#define SERVERTIMEOUT 3000          //how long to wait since last packet before marking as dorment
#define MIDIMESSAGESIZE 3           //the size in bytes of a midi message
#define RECONNECTDELAY 3000         //time waited before attempting to reconnect to server
#define PRUNINGINTERVAL 5000        //how often to check for dead clients
#define IDLETIMEOUT 300000          //how long to wait before the server auto shuts down
#define MAXCONNECTATTEMPTS 5        //attempt to connect to server no more than n times
#define PINGPACKETINTERVAL 100      //how often to send a packet to determine ping

#ifdef Q_OS_LINUX
#define AUDIODRIVER "pulseaudio"
#else
#define AUDIODRIVER "portaudio"
#endif

#define GETTIME() QDateTime::currentDateTime().toMSecsSinceEpoch()

typedef quint16 instrument_args_t;
typedef qint32 room_id_t;
typedef quint32 client_id_t;

namespace RoomCommon
{
    Q_NAMESPACE;

    enum MidiMessageType: quint8            //what the first 4 digits of a midi message mean: calculated as message[0]>>4
    {
        NOTE_OFF=0b1000,                    //note off event
        NOTE_ON=0b1001,                     //note on event
        POLYPHONIC_KEY_PRESSURE=0b1010,     //Polyphonic Key Pressure
        CONTROL_CHANGE=0b1011,              //Control Change
        PROGRAM_CHANGE=0b1100,              //Program Change
        CHANNEL_PRESSURE_CHANGE=0b1101,     //Channel Pressure
        PITCH_BEND_CHANGE=0b1110,           //Pitch Bend Change
    };

    enum PacketType: qint8
    {
        CONNECT,           //sent to request connection
        INIT,              //sent upon connection
        HEARTBEAT,         //sent constantly to ensure continued connection
        MIDI,              //a packet which contains midi data
        DISABLE,ENABLE,    //notification of a client being enabled or disabled
        DISCONNECT,        //sent to inform a player they have been disconnected
        CLOSESERVER,       //can be sent by the owner to close the server
        CHECKTIME,         //return local time
        WHOOPSIE=-1,       //if something goes really wrong | apparantly windows.h defines an enum called error
        PING=-2,           //sent by the httpapi to check if the server is alive
        REFRESHSERVERS=-3,    //sent when the servers config has changed
    }; Q_ENUM_NS(PacketType)

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
        DRUM,

    }; Q_ENUM_NS(InstrumentType);

    struct ConnectPacket
    {
        PacketType packetType=CONNECT;
        quint16 version=VERSION;
        client_id_t secretId;
        InstrumentType instrument;
        instrument_args_t instrumentArgs;
    };

    struct InitPacket
    {
        PacketType packetType=INIT;
        client_id_t clientId;
        qint64 timestamp;
    };

    struct HeartbeatPacket
    {
        PacketType packetType=HEARTBEAT;
        client_id_t secretId=-1;
    };

    struct MidiPacket
    {
        PacketType packetType=MIDI;
        client_id_t clientId;
        qint64 timestamp;
        quint8 message[3] = {0,0,0};
    };

    struct DisablePacket
    {
        PacketType packetType=DISABLE;
        client_id_t clientId;
    };

    struct EnablePacket
    {
        PacketType packetType=ENABLE;
        client_id_t clientId;
        InstrumentType instrument;
        instrument_args_t instrumentArgs=0;     //use of fields will vary by instrament.
                                                //for example 0 and 1 will be min and max note for piano,
                                                //or each could represent a single guitar string's min note.
                                                //see pianoargs for more info
    };

    struct DisconnectPacket
    {
        PacketType packetType=DISCONNECT;
        client_id_t secretId;
    };

    struct CloseServerPacket
    {
        PacketType packetType=CLOSESERVER;
        client_id_t secretId;
    };

    struct CheckTimePacket
    {
        PacketType packetType=CHECKTIME;
        qint64 clientTime;
        qint64 serverTime=0;
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

    struct RefreshServersPacket
    {
        PacketType packetType=REFRESHSERVERS;
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
        {CHECKTIME,sizeof (CheckTimePacket)},
        {WHOOPSIE, sizeof (WhoopsiePacket)},
        {PING, sizeof (PingPacket)},
        {REFRESHSERVERS, sizeof(RefreshServersPacket)},
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

    enum GuitarTuning: quint8
    {
        STANDARD,
        DADGAD,
    };
    Q_ENUM_NS(GuitarTuning);

    struct GuitarArgs
    {
        GuitarTuning tuning=STANDARD;
    };

    enum DrumLayout: quint8
    {
        AMMOON,
    };
    Q_ENUM_NS(DrumLayout);

    struct DrumArgs
    {
        DrumLayout layout=(DrumLayout)0;
    };
}
#endif
