#include "midihandler.h"
#include "ui_midihandler.h"

#include <QDebug>

using namespace RoomCommon;

MidiHandler::MidiHandler(QWidget *parent):
    QWidget(parent),
    ui(new Ui::MidiHandler)
{
    ui->setupUi(this);

    QSettings prefs;
    soundfont=prefs.value("soundfont").toString();
    audioDriver=prefs.value("audioDriver").toString();

    addSynth();
}

MidiHandler::~MidiHandler()
{
    deleteAllSynth();

    for(InstrumentView *v: instrumentViews)
        delete v;

    delete ui;
}

void MidiHandler::handleMidi(quint32 clientId, quint8 *midiMessage, qint16 latency)
{
    quint8 channelMapping=channelMap.get(clientId);
    quint8 channel=((channelMapping%16)<<4)+midiMessage[0]%16;
    fluid_synth_t *synth=synths[channelMapping>>4];

    switch(midiMessage[0]>>4)
    {
    case 0b1000:    //note off event
        fluid_synth_noteoff(synth,channel,midiMessage[1]);
        break;

    case 0b1001:    //note on event
        if(midiMessage[2]==0) {
            fluid_synth_noteoff(synth,channel,midiMessage[1]);
        } else if(latency>SERVERHEARTBEATTIMEOUT) {
            qDebug() << "Midi packet dropped from" << clientId;
        } else {
            fluid_synth_noteon(synth,channel,midiMessage[1],midiMessage[2]);
            instrumentViews[clientId]->playNote(midiMessage[1]);
        }
        break;

    case 0b1010:    //Polyphonic Key Pressure
        fluid_synth_key_pressure(synth,channel,midiMessage[1],midiMessage[2]);
        break;

    case 0b1011:    //Control Change
        fluid_synth_cc(synth,channel,midiMessage[1],midiMessage[2]);
        break;

    case 0b1100:    //Program Change
        fluid_synth_program_change(synth,channel,midiMessage[1]);
        break;

    case 0b1101:    //Channel Pressure
        fluid_synth_channel_pressure(synth,channel,midiMessage[1]);
        break;

    case 0b1110:    //Pitch Bend Change
        int value=midiMessage[1]+(midiMessage[2]<<7);
        fluid_synth_pitch_bend(synth,channel,value);
        break;
    }

    QString m;
    for(unsigned int i=0; i<MIDIMESSAGESIZE; i++) m.append(QString::number(midiMessage[i])+":");
    qDebug() << clientId << m << channel << latency<<"ms";
}

void MidiHandler::setSoundFont(QString soundfont)
{
    this->soundfont=soundfont;
    for(fluid_synth_t *synth: synths)
        fluid_synth_sfload(synth,soundfont.toUtf8().constData(),true);
}

QMetaEnum MidiHandler::instrumentTypeEnum=QMetaEnum::fromType<InstrumentType>();

void MidiHandler::setInstrumentArgs(QSettings *prefs, InstrumentType type, instrument_args_t args)
{
    prefs->setValue(QString("instrumentArgs")+instrumentTypeEnum.valueToKey(type),QString::number(args,16));
}

instrument_args_t MidiHandler::getInstrumentArgs(QSettings *prefs, InstrumentType type)
{
    instrument_args_t args = prefs->value(QString("instrumentArgs")+instrumentTypeEnum.valueToKey(type)).toString().toULongLong(nullptr,16);
    return args!=0? args:getDefaultInstrumentArgs(type);
}

instrument_args_t MidiHandler::getDefaultInstrumentArgs(InstrumentType type)
{
#define CASEFOR(CASE,TYPE) case CASE:\
{\
    TYPE a;\
    TYPE *b=(TYPE*)&args;\
    *b=a;\
    break;\
}


    instrument_args_t args=0;

    switch(type)
    {
    CASEFOR(PIANO,PianoArgs);
    CASEFOR(GUITAR,GuitarArgs);
    }

    return args;
}

void MidiHandler::setAudioDriver(QString audioDriver)
{
    this->audioDriver=audioDriver;
    for(fluid_audio_driver_t *driver: soundout)
        delete_fluid_audio_driver(driver);

    soundout.clear();

    for(fluid_synth_t *synth: synths)
    {
        fluid_settings_t* fluidSettings = fluid_synth_get_settings(synth);
        fluid_audio_driver_t* driver = new_fluid_audio_driver(fluidSettings,synth);
        soundout.append(driver);
    }

}

void MidiHandler::addSynth()
{
    fluid_audio_driver_t* driver;
    fluid_synth_t* synth;
    fluid_settings_t* fluidSettings=new_fluid_settings();

    fluid_settings_setstr(fluidSettings,"audio.driver",audioDriver.toUtf8().constData());
    fluid_settings_setstr(fluidSettings,"audio.jack.id","VirtualConcertHall");
    fluid_settings_setint(fluidSettings,"synth.midi-channels",256); //16 users per synth with 16 channels each
    fluid_settings_setint(fluidSettings,"synth.polyphony",65535);   //so there are enough notes to go around
    synth=new_fluid_synth(fluidSettings);
    driver=new_fluid_audio_driver(fluidSettings,synth);
    fluid_synth_sfload(synth,soundfont.toUtf8().constData(),true);

    synths.append(synth);
    soundout.append(driver);
}

void MidiHandler::deleteSynth()
{
    int synthIndex=synths.size()-1;
    fluid_audio_driver_t* driver=soundout.last();
    fluid_synth_t* synth=synths.last();
    fluid_settings_t* fluidSettings=fluid_synth_get_settings(synth);

    quint8 SIZE=16*synths.size();
    for(quint8 i=SIZE; i<16; i++)
    {
        if(channelMap.value_contains(i))
        {
            for(quint8 j=0; i<SIZE; i++)
            {
                if(!channelMap.value_contains(j))
                {
                    quint32 clientid=channelMap.xget(i);
                    channelMap.remove(clientid);
                    channelMap.set(clientid,j);

                    break;
                }
            }

            qDebug() << "Cannot shrink synth pool: too many orpheans created";
            return;
        }
    }

    delete_fluid_audio_driver(driver);
    delete_fluid_synth(synth);
    delete_fluid_settings(fluidSettings);
}

void MidiHandler::deleteAllSynth()
{
    for(fluid_audio_driver_t* driver: soundout)
        delete_fluid_audio_driver(driver);
    for(fluid_synth_t* synth: synths)
    {
        fluid_settings_t* settings=fluid_synth_get_settings(synth);
        delete_fluid_synth(synth);
        delete_fluid_settings(settings);
    }

    channelMap.clear();
}

void MidiHandler::addChannel(quint32 clientId, InstrumentType instrument, quint64 args, QWidget *parent)
{
    InstrumentView *v;

    //add visual
    if(!instrumentViews.contains(clientId)) {
        v=new InstrumentView(instrument,args,this);
        v->updateInstrument();
        ui->gridLayout_2->addWidget(v);

        instrumentViews.insert(clientId,v);
    } else {
        v=instrumentViews[clientId];
    }
    v->show();

    //add audio
    if(!channelMap.key_contains(clientId))
    {
        int SIZE=16*synths.size();

        for(int i=0; i<SIZE; i++)
        {
            if(!channelMap.value_contains(i))
            {
                channelMap.set(clientId,i);

                return;
            }
        }

        addSynth();
        channelMap.set(clientId,SIZE);
    }
}

void MidiHandler::delChannel(quint32 clientId)
{
    InstrumentView *v = instrumentViews[clientId];
    if(v!=nullptr) v->hide();
    instrumentViews.remove(clientId);

    channelMap.remove(clientId);
}
