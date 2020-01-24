#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H

#include <QSettings>
#include <QtGlobal>
#include <QTimer>

#include <fluidsynth.h>

#include "bihash.h"
#include "instrumentview.h"
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

    void addChannel(quint32 clientId, InstrumentType instrument, quint64 args, QWidget *parent=nullptr),delChannel(quint32 clientId);
    void handleMidi(quint32 clientId, quint8* midiMessage, qint16 latency);
    void setSoundFont(QString soundfont), setAudioDriver(QString audioDriver);

    static void setInstrumentArgs(QSettings *prefs, InstrumentType type, instrument_args_t args);
    static instrument_args_t getInstrumentArgs(QSettings *prefs, InstrumentType type);
    static instrument_args_t getDefaultInstrumentArgs(InstrumentType type);

private:    //methods
    void addSynth(), deleteSynth(), deleteAllSynth();

private:
    Ui::MidiHandler *ui;

    QList<fluid_synth_t*> synths;
    QList<fluid_audio_driver_t*> soundout;
    BiHash<quint32,quint8> channelMap;
    QHash<quint32,InstrumentView*> instrumentViews;

    QString soundfont;
    QString audioDriver;

    InstrumentType insturmentType;
    quint64 instrumentArgs=0;

    static QMetaEnum instrumentTypeEnum;
};

#endif // MIDIHANDLER_H
