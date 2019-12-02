#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H

#include <RtMidi.h>

#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>

#include <QSettings>

class MidiHandler: public QObject
{
    Q_OBJECT

public:
    MidiHandler(QUdpSocket *qSocket);
    ~MidiHandler();

private:
    RtMidiIn midiin;

    QUdpSocket *qSocket;
    QHostAddress server;
    unsigned int serverPort;

    QSettings prefs;

    static void handleMidi( double timeStamp, std::vector<unsigned char> *message, void *userData );
};

#endif // MIDIHANDLER_H
