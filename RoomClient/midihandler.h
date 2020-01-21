#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H

#include <RtMidi.h>
#include <fluidsynth.h>

#include <QtGlobal>
#include <QUdpSocket>
#include <QTimer>
#include <QNetworkDatagram>

#include <QSettings>
#include "roomcommon.h"

#ifdef Q_OS_WIN
#define AUDIODRIVER "dsound"
#endif

#ifdef Q_OS_LINUX
#define AUDIODRIVER "pulseaudio"
#endif


using namespace RoomCommon;

class MidiHandler: public QObject
{
    Q_OBJECT

public:
    MidiHandler(quint32 secretId,QString ipaddress="127.0.0.1",quint16 port=1998, QObject *parent=nullptr);
    ~MidiHandler();

    void closeServer();

signals:
    void playerJoin(quint32 clientId, InstrumentType type, quint64 args);
    void playerLeave(quint32 clientId);
    void midiMessage(quint32 clientId, quint8* message);

private slots:
    void handleDataFromServer();
    void attemptConnect();
    void iterateServertime();

private:    //methods
    static void handleMidi( double timeStamp, std::vector<unsigned char> *message, void *userData );
    void handleMidiFromServer(quint32 clientId,qint64 timestamp, quint8* midiMessage);
    void disconnectFromServer();
    void loadInstrumentConfig(QSettings *prefs);

    void addSynth(quint32 clientId),delSynth(quint32 clientId);

private:
    RtMidiIn midiin;
    QHash<quint32,fluid_synth_t*> midiout;
    QHash<quint32,fluid_audio_driver_t*> soundout;

    QUdpSocket qSocket;
    QTimer reconnectClock;
    QTimer serverTimeIterator;

    QHostAddress serverHost;
    quint16 serverPort;
    QString soundfont;
    qint64 timestamp;
    quint32 clientId=-1,secretId;
    quint8 reconnectAttempts=MAXCONNECTATTEMPTS;
    InstrumentType insturmentType;
    quint64 instrumentArgs=0;
};

#endif // MIDIHANDLER_H
