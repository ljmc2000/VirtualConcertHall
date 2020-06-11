#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H

#include <QSettings>
#include <QtGlobal>
#include <QTimer>

#include <fluidsynth.h>

#include "bihash.h"
#include "userview.h"
#include "replaylogger.h"
#include "roomcommon.h"

#define AUDIODRIVER "portaudio"


using namespace RoomCommon;

namespace Ui {
class MidiHandler;
}

class MidiHandler: public QWidget
{
    Q_OBJECT

public:
    MidiHandler(QWidget *parent=nullptr);
    ~MidiHandler();
    void resizeEvent(QResizeEvent *event) override;

    void addChannel(client_id_t clientId, QString username, InstrumentType instrument, instrument_args_t args, QWidget *parent=nullptr),delChannel(client_id_t clientId);
    void handleMidi(client_id_t clientId, quint8* midiMessage, qint16 latency);
    void setSoundFont(QString soundfont), setAudioDevice(QString audioDevice);
    void reorganizeInstrumentViews();
    void setUsername(client_id_t clientId, QString username);

    static void setInstrumentArgs(QSettings *prefs, InstrumentType type, instrument_args_t args);
    static instrument_args_t getInstrumentArgs(QSettings *prefs, InstrumentType type);
    static instrument_args_t getDefaultInstrumentArgs(InstrumentType type);

public slots:
    void toggleRecording(bool on);

public: //attributes
    static const char* audioDriver;
    static const char* driverDotDevice;
    qint16 maxLatency = 50;

private:    //methods
    void initSynth(), deleteSynth(), reinitialize();
    void shuffleChannels();

private:
    Ui::MidiHandler *ui;

    fluid_synth_t* synth;
    fluid_audio_driver_t* soundout;
    QHash<client_id_t,quint8> channelMap;
    QHash<client_id_t,UserView*> instrumentViews;
    ReplayLogger replayLogger;
    quint8 peruser;

    QString soundfont;
    QString audioDevice;

    InstrumentType insturmentType;
    instrument_args_t instrumentArgs=0;

    static QMetaEnum instrumentTypeEnum;
    static QHash<InstrumentType,quint16> InstrumentSounds;
};

#endif // MIDIHANDLER_H
