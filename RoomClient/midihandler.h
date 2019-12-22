#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H

#include <RtMidi.h>

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QNetworkDatagram>

#include <QSettings>

class MidiHandler: public QObject
{
    Q_OBJECT

public:
    MidiHandler();
    ~MidiHandler();

public slots:
    void handleDataFromServer();
    void heartBeat();

private:
    RtMidiIn midiin;
    RtMidiOut midiout;

    QUdpSocket qSocket;
    QTimer heartBeatClock;

    static void handleMidi( double timeStamp, std::vector<unsigned char> *message, void *userData );
};

#endif // MIDIHANDLER_H
