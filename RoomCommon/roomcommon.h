#define HEARTBEATINTERVAL 40
#define SERVERHEARTBEATTIMEOUT HEARTBEATINTERVAL*3
#define PACKETHEADERSIZE 2
#define RECONNECTDELAY 3000

namespace RoomCommon
{
    enum PacketType {INIT,HEARTBEAT,MIDI,DISCONNECT};

    static char disconnectPayload[]={DISCONNECT};
}
