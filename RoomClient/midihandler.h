#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H

#include <QSettings>
#include <QtGlobal>
#include <QTimer>

#include <fluidsynth.h>

#include "bihash.h"
#include "userview.h"
#include "roomcommon.h"


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

    void addChannel(client_id_t clientId, InstrumentType instrument, instrument_args_t args, QWidget *parent=nullptr),delChannel(client_id_t clientId);
    void handleMidi(client_id_t clientId, quint8* midiMessage, qint16 latency);
    void setSoundFont(QString soundfont), setAudioDriver(QString audioDriver);
    void reorganizeInstrumentViews();

    static void setInstrumentArgs(QSettings *prefs, InstrumentType type, instrument_args_t args);
    static instrument_args_t getInstrumentArgs(QSettings *prefs, InstrumentType type);
    static instrument_args_t getDefaultInstrumentArgs(InstrumentType type);

    qint16 maxLatency = 50;

private:    //methods
    void addSynth(), deleteSynth(), deleteAllSynth();

private:
    Ui::MidiHandler *ui;

    QList<fluid_synth_t*> synths;
    QList<fluid_audio_driver_t*> soundout;
    BiHash<client_id_t,quint8> channelMap;
    QHash<client_id_t,UserView*> instrumentViews;

    QString soundfont;
    QString audioDriver;

    InstrumentType insturmentType;
    instrument_args_t instrumentArgs=0;

    static QMetaEnum instrumentTypeEnum;
    static QHash<InstrumentType,quint16> InstrumentSounds;
};

#endif // MIDIHANDLER_H
