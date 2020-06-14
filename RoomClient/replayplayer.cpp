#include "replayplayer.h"

#include <QDebug>
#include <QFileDialog>
#include <QSettings>

using namespace RoomCommon;

ReplayPlayer::ReplayPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReplayPlayer)
{
    ui->setupUi(this);

    connect(ui->backButton,&QPushButton::clicked,
            [=](){ emit switchScreen(MAINMENU); });

    connect(ui->pickReplayButton, SIGNAL(clicked()),
            this, SLOT(selectReplay()));

    connect(ui->PlayButton, SIGNAL(clicked()),
            this, SLOT(playPause()));

    QSettings settings;
    soundfont=settings.value("soundfont").toString();
}

ReplayPlayer::~ReplayPlayer()
{
    delete ui;
}

void ReplayPlayer::selectReplay()
{
    ui->pickReplayButton->setText(QFileDialog::getOpenFileName(this, "Replay location", "", REPLAY_FILE_DESC));
}

void ReplayPlayer::playPause()
{
    t=new ReplayPlayerThread(ui->pickReplayButton->text(),soundfont);
    connect(t, &QThread::finished, t, &QObject::deleteLater);
    t->start();
}

ReplayPlayerThread::ReplayPlayerThread(QString filename, QString soundfont)
{
    this->filename=filename;

    fluid_settings_t *s=new_fluid_settings();
    CONFIGURE_SYNTH(s);
    synth=new_fluid_synth(s);
    driver=new_fluid_audio_driver(s,synth);
    fluid_synth_sfload(synth,soundfont.toUtf8().constData(),true);
}

ReplayPlayerThread::~ReplayPlayerThread()
{
    fluid_settings_t *s=fluid_synth_get_settings(synth);
    delete_fluid_audio_driver(driver);
    delete_fluid_synth(synth);
    delete_fluid_settings(s);
}

void ReplayPlayerThread::run()
{
    ReplayLogChunk chunk;
    QFile in(filename);
    in.open(QIODevice::ReadOnly);

    //first
    {
        in.read((char*)&chunk,sizeof(ReplayLogChunk));
        chunk.sleep=0;
        playNote(chunk);
    }

    while(!in.atEnd())
    {
        in.read((char*)&chunk,sizeof(ReplayLogChunk));
        playNote(chunk);
    }
}

void ReplayPlayerThread::playNote(ReplayLogChunk chunk)
{
    msleep(chunk.sleep);

    switch (chunk.midiMessage[0]>>4)
    {
    case NOTE_OFF:
        fluid_synth_noteoff(synth,chunk.channel,chunk.midiMessage[1]);
        break;
    case NOTE_ON:
        fluid_synth_noteon(synth,chunk.channel,chunk.midiMessage[1],chunk.midiMessage[2]);
        break;
    case POLYPHONIC_KEY_PRESSURE:
        fluid_synth_key_pressure(synth,chunk.channel,chunk.midiMessage[1],chunk.midiMessage[2]);
        break;
    case CONTROL_CHANGE:
        fluid_synth_cc(synth,chunk.channel,chunk.midiMessage[1],chunk.midiMessage[2]);
        break;
    case PROGRAM_CHANGE:
        fluid_synth_program_change(synth,chunk.channel,chunk.midiMessage[1]);
        break;
    case CHANNEL_PRESSURE_CHANGE:
        fluid_synth_channel_pressure(synth,chunk.channel,chunk.midiMessage[1]);
        break;
    case PITCH_BEND_CHANGE:
        int value=chunk.midiMessage[1]+(chunk.midiMessage[2]<<7);
        fluid_synth_pitch_bend(synth,chunk.channel,value);
        break;
    }
}
