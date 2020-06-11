#include "replaylogger.h"

ReplayLogger::ReplayLogger(QObject *parent) : QObject(parent)
{
}

ReplayLogger::~ReplayLogger()
{
    if(enabled) disable();
}

void ReplayLogger::enable(QString filename)
{
    out = new QFile(filename+".vchm"+REPLAY_FORMAT_VERSION);
    out->open(QIODevice::WriteOnly);
    enabled=true;
}

void ReplayLogger::disable()
{
    out->close();
    enabled=false;
}

void ReplayLogger::handleMidi(quint8 channel, quint8 midiMessage[3])
{
    if(!enabled) return;
    ReplayLogChunk chunk;
    chunk.channel=channel;
    for(int i=0; i<3; i++) chunk.midiMessage[i]=midiMessage[i];

    quint64 newTime=GETTIME();
    chunk.sleep=newTime-time;
    time=newTime;

    out->write((char*)&chunk,sizeof(ReplayLogChunk));
}
