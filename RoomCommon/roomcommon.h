#define HEARTBEATINTERVAL 40
#define SERVERHEARTBEATTIMEOUT 50
#define MIDIMESSAGESIZE 3       //the size in bytes of a midi message
#define RECONNECTDELAY 3000     //time waited before attempting to reconnect to server
#define PRUNINGINTERVAL 5000    //how often to check for dead clients

namespace RoomCommon
{
    enum PacketType {CONNECT,       //sent to request connection
                     INIT,          //sent upon connection
                     HEARTBEAT,     //sent constantly to ensure continued connection
                     MIDI,          //a packet which contains midi data
                     UPDATENUMBER,  //every client has a number. said numbers are shuffled after a disconnect
                     DISCONNECT     //sent to inform a player they have been disconnected
                    };

    struct ConnectPacket
    {
        PacketType packetType=CONNECT;
    };
    static ConnectPacket connectPacket;

    struct InitPacket
    {
        PacketType packetType=INIT;
        quint8 clientId;
        qint64 timestamp;
    };

    struct HeartbeatPacket
    {
        PacketType packetType=HEARTBEAT;
        qint64 timestamp;
    };

    struct MidiPacket
    {
        PacketType packetType=MIDI;
        quint8 clientId;
        qint64 timestamp;
        quint8 message[3] = {0,0,0};
    };

    struct UpdateNumberPacket
    {
        PacketType packetType=UPDATENUMBER;
        quint8 clientId;
    };

    struct DisconnectPacket
    {
        PacketType packetType=DISCONNECT;
    };
    static DisconnectPacket disconnectPacket;
}
