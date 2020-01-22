#include "settingswindow.h"
#include "ui_settingswindow.h"
#include <iostream>
#include <QObject>
#include <QPushButton>
#include <QFileDialog>
#include "fluidsynth.h"

using namespace OnlineStatusNamespace;

SettingsWindow::SettingsWindow(HttpAPIClient *httpApiClient, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    this->httpApiClient=httpApiClient;

    instrumentType=(InstrumentType)prefs.value("instrumentType").toInt();
    renderInstrument();

    setMidiPortsList();

    fluid_settings_t *settings=new_fluid_settings();
    fluid_settings_foreach_option(settings,"audio.driver",this,&SettingsWindow::setDriverList);
    delete_fluid_settings(settings);

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
    delete ui;
}

void SettingsWindow::setMidiPortsList()
{
    ui->midiInputSelector->clear();

    int inport=prefs.value("midiInPort").toInt(),outport=prefs.value("midiOutPort").toInt();

    midiin.openPort(inport<midiin.getPortCount() ? inport:0);
    midiin.setCallback(midiHandler,this);
    midiout.openPort(outport<midiin.getPortCount() ? outport:0);

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
    prefs.setValue("soundfont",filename);
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

    switch(message->at(0))
    {
        case 144:
            int note=message->at(1);
            if(note<self->minNote)
            {
                self->minNote=note;
                self->prefs.setValue("minNote",note);
            }
            if(note>self->maxNote)
            {
                self->maxNote=note;
                self->prefs.setValue("maxNote",note);
            }
            break;
    }

    self->renderInstrument();
}

void SettingsWindow::setDriverList(void *data, const char *name, const char* type)
{
    SettingsWindow *self=(SettingsWindow*)data;
    self->ui->audioDriverBox->addItem(type);
}
