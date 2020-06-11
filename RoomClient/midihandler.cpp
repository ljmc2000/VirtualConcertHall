#include "midihandler.h"
#include "ui_midihandler.h"

#include <QDebug>
#include <QFileDialog>
#include <QPushButton>
#include <math.h>

using namespace RoomCommon;

const char* MidiHandler::audioDriver="portaudio";
const char* MidiHandler::driverDotDevice="audio.portaudio.device";

MidiHandler::MidiHandler(QWidget *parent):
    QWidget(parent),
    ui(new Ui::MidiHandler)
{
    ui->setupUi(this);

    QSettings prefs;
    this->soundfont=prefs.value("soundfont").toString();
    this->audioDevice=prefs.value("audioDevice").toString();
    initSynth();
}

MidiHandler::~MidiHandler()
{
    deleteSynth();

    for(UserView *u: instrumentViews)
        delete u;

    delete ui;
}

void MidiHandler::resizeEvent(QResizeEvent *event)
{
    reorganizeInstrumentViews();
}

void MidiHandler::handleMidi(client_id_t clientId, quint8 *midiMessage, qint16 latency)
{
    MidiMessageType instruction=(MidiMessageType)(midiMessage[0]>>4);
    quint8 channel=midiMessage[0]%16;
    if(channel>peruser) return;
    channel+=channelMap.value(clientId);
    UserView *u = instrumentViews.value(clientId);
    if(u==nullptr) return;

    u->setLatency(latency);

    switch(instruction)
    {
    case NOTE_OFF:
        fluid_synth_noteoff(synth,channel,midiMessage[1]);
        break;

    case NOTE_ON:
        if(midiMessage[2]==0) {
            fluid_synth_noteoff(synth,channel,midiMessage[1]);
        } else if(latency>maxLatency) {
            qDebug() << "Midi packet dropped from" << clientId;
        } else {
            fluid_synth_noteon(synth,channel,midiMessage[1],u->volume*midiMessage[2]);
            u->playNote(midiMessage[1]);
        }
        break;

    case POLYPHONIC_KEY_PRESSURE:
        fluid_synth_key_pressure(synth,channel,midiMessage[1],midiMessage[2]);
        break;

    case CONTROL_CHANGE:
        fluid_synth_cc(synth,channel,midiMessage[1],midiMessage[2]);
        break;

    case PROGRAM_CHANGE:
        fluid_synth_program_change(synth,channel,midiMessage[1]);
        break;

    case CHANNEL_PRESSURE_CHANGE:
        fluid_synth_channel_pressure(synth,channel,midiMessage[1]);
        break;

    case PITCH_BEND_CHANGE:
        int value=midiMessage[1]+(midiMessage[2]<<7);
        fluid_synth_pitch_bend(synth,channel,value);
        break;
    }

    QString m;
    for(unsigned int i=0; i<MIDIMESSAGESIZE; i++) m.append(QString::number(midiMessage[i])+":");
    replayLogger.handleMidi(channel,midiMessage);
    qDebug() << clientId << m << channel << latency<<"ms";
}

void MidiHandler::setSoundFont(QString soundfont)
{
    this->soundfont=soundfont;
    fluid_synth_sfload(synth,soundfont.toUtf8().constData(),true);
}

void MidiHandler::reorganizeInstrumentViews()
{
    QSizeF syze=size();
    int x=0,y=0;
    //int perSide=(syze.width()/syze.height())*instrumentViews.size();
    int perSide=sqrt(instrumentViews.size());

    for(UserView *view: instrumentViews)
    {
        if(x==perSide)
        {
            y++;
            x=0;
        }
        ui->flowLayout->addWidget(view,x,y);
        x++;
    }
}

QMetaEnum MidiHandler::instrumentTypeEnum=QMetaEnum::fromType<InstrumentType>();
QHash<InstrumentType,quint16> MidiHandler::InstrumentSounds
{
    {PIANO,0},  //0 0
    {GUITAR,25},    //0 25
    {DRUM,32768},   //128 0
};

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
    CASEFOR(DRUM,DrumArgs);
    }

    return args;
}

void MidiHandler::setAudioDevice(QString audioDevice)
{
    this->audioDevice=audioDevice;
    if(audioDevice=="") return;

    if(audioDevice.length()!=0)
    {
        fluid_settings_t* fluidSettings = fluid_synth_get_settings(synth);
        fluid_settings_setstr(fluidSettings,driverDotDevice,audioDevice.toUtf8().constData());
    }
    reinitialize();
}

void MidiHandler::initSynth()
{
    fluid_settings_t* fluidSettings=new_fluid_settings();

    fluid_settings_setstr(fluidSettings,"audio.driver",AUDIODRIVER);
    fluid_settings_setint(fluidSettings,"synth.midi-channels",256); //16 users per synth with 16 channels each
    fluid_settings_setint(fluidSettings,"synth.polyphony",65535);   //so there are enough notes to go around

    if(audioDevice.length()!=0)
    {
        fluid_settings_setstr(fluidSettings,driverDotDevice,audioDevice.toUtf8().constData());
    }

    synth=new_fluid_synth(fluidSettings);
    soundout=new_fluid_audio_driver(fluidSettings,synth);
    fluid_synth_sfload(synth,soundfont.toUtf8().constData(),true);
}

void MidiHandler::deleteSynth()
{
    delete_fluid_audio_driver(soundout);
    fluid_settings_t* settings=fluid_synth_get_settings(synth);
    delete_fluid_synth(synth);
    delete_fluid_settings(settings);
}

void MidiHandler::reinitialize()
{
    deleteSynth();
    initSynth();
}

void MidiHandler::addChannel(client_id_t clientId, QString username, InstrumentType instrument, instrument_args_t args, QWidget *parent)
{
    InstrumentView *v;
    UserView *u;

    //add visual
    if(!instrumentViews.contains(clientId)) {
        v=InstrumentView::getInstrumentView(instrument,args,this);
        v->updateInstrument();

        u=new UserView(v,username,this);
        instrumentViews.insert(clientId,u);
    } else {
        u=instrumentViews[clientId];
    }
    u->show();
    reorganizeInstrumentViews();

    //add audio
    shuffleChannels();
}

void MidiHandler::delChannel(client_id_t clientId)
{
    UserView *u = instrumentViews[clientId];
    if(u!=nullptr) u->deleteLater();
    instrumentViews.remove(clientId);

    shuffleChannels();
    reorganizeInstrumentViews();
}

void MidiHandler::shuffleChannels()
{
    channelMap.clear();
    peruser=256/instrumentViews.size();
    if(peruser>16) peruser=16;
    int channel=0;

    for(QHash<client_id_t,UserView*>::iterator i=instrumentViews.begin(); i!=instrumentViews.end(); i++)
    {
        channelMap[i.key()]=channel;
        quint16 isound=InstrumentSounds[i.value()->getInstrumentType()];
        quint8 *esound=(quint8*)&isound;
        qint32 sfont_id;

        for(int i=0; i<peruser; i++)
        {
            int a,b;
            fluid_synth_get_program(synth,channel,&sfont_id,&a,&b);
            fluid_synth_program_select(synth,channel,sfont_id,esound[1],esound[0]);
            channel++;
        }
    }
}

void MidiHandler::setUsername(client_id_t clientId, QString username)
{
    UserView *u=instrumentViews[clientId];
    u->setUsername(username);
}

void MidiHandler::toggleRecording(bool on)
{
    if(on){
        replayLogger.enable(QFileDialog::getSaveFileName(this, "Replay location", "", REPLAY_FILE_DESC));
    } else {
        replayLogger.disable();
    }
}
