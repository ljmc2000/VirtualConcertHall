#ifndef REPLAYLOGGER_H
#define REPLAYLOGGER_H

#include <QObject>
#include <QDateTime>
#include <QFile>

#include "replay.h"
#include "roomcommon.h"

using namespace RoomCommon;

class ReplayLogger : public QObject
{
    Q_OBJECT

public:
    explicit ReplayLogger(QObject *parent = nullptr);
    ~ReplayLogger();

    void enable(QString filename);
    void disable();
    void handleMidi(quint8 channel, quint8 midiMessage[3]);

public: //attrs
    bool enabled=false;

private:
    QFile *out;
    quint64 time=GETTIME();
};

#endif // REPLAYLOGGER_H
