#include "settingswindow.h"
#include "ui_settingswindow.h"

#include <QFileDialog>
#include <QObject>
#include <QPushButton>

using namespace OnlineStatusNamespace;

SettingsWindow::SettingsWindow(HttpAPIClient *httpApiClient, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    this->httpApiClient=httpApiClient;
    if(prefs.value("midiInPort").isNull()) setDefaults();

    instrumentType=(InstrumentType)prefs.value("instrumentType").toInt();
    renderInstrument();

    setMidiPortsList();

    fluid_settings_t *settings=new_fluid_settings();
    fluid_settings_foreach_option(settings,"audio.driver",this,&SettingsWindow::setDriverList);
    delete_fluid_settings(settings);
    ui->audioDriverBox->setCurrentText(prefs.value("audioDriver").toString());

    ui->pickSfButton->setText(prefs.value("soundfont").toString());

    initSynth();

    refreshUsername();

    connect(ui->backButton, &QPushButton::clicked,
            [=](){emit switchScreen(MAINMENU);});

    connect(ui->midiInputSelector, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setMidiInPort()));

    connect(ui->pickSfButton, SIGNAL(clicked()),
            this, SLOT(setSoundFont()));

    connect(ui->audioDriverBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setAudioDriver()));
}

SettingsWindow::~SettingsWindow()
{
    closeSynth();
    delete ui;
}

void SettingsWindow::setMidiPortsList()
{
    ui->midiInputSelector->clear();

    int inport=prefs.value("midiInPort").toInt();

    midiin.openPort(inport<midiin.getPortCount() ? inport:0);
    midiin.setCallback(midiHandler,this);

    for(unsigned int i=0; i<midiin.getPortCount(); i++)
    {
        ui->midiInputSelector->addItem(midiin.getPortName(i).c_str());
    }

    QMetaEnum e = QMetaEnum::fromType<InstrumentType>();
    for(int i=0; i<e.keyCount(); i++)
    {
        ui->instrumentTypeBox->addItem(e.valueToKey(i));
    }

    ui->midiInputSelector->setCurrentIndex(prefs.value("midiInPort").toInt());
    ui->instrumentTypeBox->setCurrentIndex(prefs.value("instrumentType").toInt());
}

void SettingsWindow::renderInstrument()
{
    quint8 min=prefs.value("minNote").toInt(),max=prefs.value("maxNote").toInt();
    if(min<max && max-min>0) switch(instrumentType)
    {
    case PIANO:
        ui->instrumentView->fromPiano(min,max);
        break;
    }
}

void SettingsWindow::setMidiInPort()
{
    int port=ui->midiInputSelector->currentIndex();
    prefs.setValue("midiInPort",port);
    midiin.closePort();
    midiin.openPort(port);
}

void SettingsWindow::setSoundFont()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select Sound Font", "", "Sound font 2 files (*.sf2)");
    if(filename.size()!=0)
    {
        prefs.setValue("soundfont",filename);
        ui->pickSfButton->setText(filename);
        fluid_synth_sfload(midiout,filename.toUtf8().constData(),true);
    }
}

void SettingsWindow::setInstrumentType()
{
    int instrument=ui->instrumentTypeBox->currentIndex();
    prefs.setValue("instrumentType",instrument);
    this->instrumentType=(InstrumentType)instrument;
}

void SettingsWindow::setAudioDriver()
{
    prefs.setValue("audioDriver",ui->audioDriverBox->currentText());
    closeSynth();
    initSynth();
}

void SettingsWindow::logout()
{
    prefs.remove("loginToken");
    httpApiClient->signout();
}

void SettingsWindow::refreshUsername()
{
    QString username = httpApiClient->getUsername();
    if(username.size()!=0)
    {
        ui->signinLabel->setText("signed in as "+username);
        ui->signinButton->setText("sign out");
        connect(ui->signinButton, SIGNAL(clicked()),
                this, SLOT(logout()));
    }

    else
    {
        ui->signinLabel->setText("Error in httpapi");
        ui->signinButton->setText("");
        disconnect(ui->signinButton);
    }
}

void SettingsWindow::midiHandler(double timeStamp, std::vector<unsigned char> *message, void *userData)
{
    SettingsWindow *self = static_cast<SettingsWindow*>(userData);
    bool change=false;
    int note=message->at(1);

    switch(message->at(0))
    {
        case 144:
        {
            if(note<self->minNote)
            {
                change=true;
                self->minNote=note;
                self->prefs.setValue("minNote",note);
            }
            if(note>self->maxNote)
            {
                change=true;
                self->maxNote=note;
                self->prefs.setValue("maxNote",note);
            }

            if(message->at(2)!=0)self->ui->instrumentView->playNote(note);
            fluid_synth_noteon(self->midiout,0,note,message->at(2));
            break;
        }

        case 128:
            fluid_synth_noteoff(self->midiout,0,note);
            break;
    }

    if(change) self->renderInstrument();
}

void SettingsWindow::initSynth()
{
    QString filename=prefs.value("soundfont").toString();
    fluid_settings_t *settings = new_fluid_settings();
    fluid_settings_setstr(settings, "audio.driver", ui->audioDriverBox->currentText().toUtf8().constData());
    midiout=new_fluid_synth(settings);
    soundout=new_fluid_audio_driver(settings,midiout);
    if(filename.size()!=0) fluid_synth_sfload(midiout,filename.toUtf8().constData(),true);
}

void SettingsWindow::closeSynth()
{
    fluid_settings_t *settings = fluid_synth_get_settings(midiout);
    delete_fluid_audio_driver(soundout);
    delete_fluid_synth(midiout);
    delete_fluid_settings(settings);
}

void SettingsWindow::setDriverList(void *data, const char *name, const char* value)
{
    SettingsWindow *self=(SettingsWindow*)data;
    QString file="file";
    if(value != file)self->ui->audioDriverBox->addItem(value);
}

void SettingsWindow::setDefaults()
{
    ui->audioDriverBox->setCurrentIndex(0);
    ui->instrumentTypeBox->setCurrentIndex(0);
    ui->audioDriverBox->setCurrentIndex(0);

    prefs.setValue("midiInPort", 0);
    prefs.setValue("instrumentType",PIANO);
    prefs.setValue("audioDriver",ui->audioDriverBox->currentText());
    prefs.setValue("minNote",0);
    prefs.setValue("maxNote",127);
}
