#ifndef REPLAY_H
#define REPLAY_H

#include <QObject>
#define REPLAY_FORMAT_VERSION "1"
#define REPLAY_FILE_DESC "Virtual Concert Hall replay v" REPLAY_FORMAT_VERSION " files (*.vchm" REPLAY_FORMAT_VERSION ")"

struct ReplayLogChunk
{
    quint8 channel;
    quint8 midiMessage[3];
    quint32 sleep;
};

#endif // REPLAY_H
