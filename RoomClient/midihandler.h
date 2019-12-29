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
    void attemptConnect();

private:
    RtMidiIn midiin;
    RtMidiOut midiout;

    QUdpSocket qSocket;
    QHostAddress serverHost;
    quint16 serverPort;
    QTimer reconnectClock;
    qint64 timestamp;

    static void handleMidi( double timeStamp, std::vector<unsigned char> *message, void *userData );

    quint8 clientId=-1;
};

#endif // MIDIHANDLER_H
