#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H

#include <RtMidi.h>

#include <QUdpSocket>
#include <QTimer>
#include <QNetworkDatagram>

#include <QSettings>

class MidiHandler: public QObject
{
    Q_OBJECT

public:
    MidiHandler(quint32 secretId,QString ipaddress="127.0.0.1",quint16 port=1998);
    ~MidiHandler();

private slots:
    void handleDataFromServer();
    void attemptConnect();
    void iterateServertime();

private:
    RtMidiIn midiin;
    RtMidiOut midiout;

    QUdpSocket qSocket;
    QHostAddress serverHost;
    quint16 serverPort;
    QTimer reconnectClock;
    QTimer serverTimeIterator;
    qint64 timestamp;

    static void handleMidi( double timeStamp, std::vector<unsigned char> *message, void *userData );
    void handleMidiFromServer(quint32 clientId,qint64 timestamp, quint8* midiMessage);

    quint32 clientId=-1,secretId;
};

#endif // MIDIHANDLER_H
