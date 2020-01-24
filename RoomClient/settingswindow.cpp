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
    instrumentArgs=prefs.value("instrumentArgs").toString().toULongLong(nullptr,16);
    audioDriver=prefs.value("audioDriver").toString();
    soundfont=prefs.value("soundfont").toString();

    setMidiPortsList();

    fluid_settings_t *settings=new_fluid_settings();
    fluid_settings_foreach_option(settings,"audio.driver",this,&SettingsWindow::setDriverList);
    delete_fluid_settings(settings);
    ui->audioDriverBox->setCurrentText(audioDriver);

    ui->pickSfButton->setText(soundfont);

    refreshUsername();

    connect(ui->backButton, &QPushButton::clicked,
            [=](){emit switchScreen(MAINMENU);});

    connect(ui->midiInputSelector, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setMidiInPort()));

    connect(ui->pickSfButton, SIGNAL(clicked()),
            this, SLOT(setSoundFont()));

    connect(ui->audioDriverBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setAudioDriver()));

    connect(this, SIGNAL(instrumentUpdate()),
            this, SLOT(redrawInstrument()));

    ui->midiHandler->setAudioDriver(audioDriver);
    ui->midiHandler->setSoundFont(soundfont);
    ui->midiHandler->addChannel(0,instrumentType,instrumentArgs);

    showInstrumentConfig();
}

SettingsWindow::~SettingsWindow()
{
    clearInstrumentConfig();
    delete ui;
}

void SettingsWindow::setMidiPortsList()
{
    ui->midiInputSelector->clear();

    int inport=prefs.value("midiInPort").toInt();

    midiin.openPort(inport<midiin.getPortCount() ? inport:0);
    midiin.setClientName("VirtualConcertHallClient");
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
        ui->midiHandler->setSoundFont(filename);
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
    ui->midiHandler->setAudioDriver(ui->audioDriverBox->currentText());
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

#define SETARG(ARGTYPE,ARG) \
ARGTYPE* args=(ARGTYPE*)&instrumentArgs;\
changeAttr=&args->ARG;\
midiin.cancelCallback();\
midiin.setCallback(&midiSetArg,this);
void SettingsWindow::showInstrumentConfig()
{
    clearInstrumentConfig();

    switch (instrumentType)
    {
        case PIANO:
        {
            {
            QPushButton *button = new QPushButton("Set Minimum Note",this);
            connect(button, &QPushButton::clicked,[=](){
                SETARG(PianoArgs,minNote);
                ui->instrumentDebugLabel->setText("Press the lowest key on your piano");
            });
            ui->instrumentConf->addWidget(button);
            }

            {
            QPushButton *button = new QPushButton("Set Maximum Note",this);
            connect(button, &QPushButton::clicked,[=](){
                SETARG(PianoArgs,maxNote);
                ui->instrumentDebugLabel->setText("Press the highest key on your piano");
            });
            ui->instrumentConf->addWidget(button);
            }
        }
    }
}

void SettingsWindow::clearInstrumentConfig()
{
    while (true)
    {
        QLayoutItem *wItem = ui->instrumentConf->takeAt(0);
        if(wItem == 0)
            break;
        else
            delete wItem;
    }

    ui->instrumentDebugLabel->setText("");
}

void SettingsWindow::redrawInstrument()
{
    ui->midiHandler->delChannel(0);
    ui->midiHandler->addChannel(0,instrumentType,instrumentArgs);
}

void SettingsWindow::midiHandler(double timeStamp, std::vector<unsigned char> *message, void *userData)
{
    SettingsWindow *self = static_cast<SettingsWindow*>(userData);
    self->ui->midiHandler->handleMidi(0,message->data(),0);
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
    prefs.setValue("instrumentArgs",QString::number(0ull,16));
    prefs.setValue("audioDriver",ui->audioDriverBox->currentText());
}

void SettingsWindow::midiSetArg(double timeStamp, std::vector<unsigned char> *message, void *userData)
{
    SettingsWindow *self=(SettingsWindow*)userData;

    if(message->at(0)>>4 == 0b1001)
    {
        *self->changeAttr=message->at(1);
        self->changeAttr=0;
        self->prefs.setValue("instrumentArgs",QString::number(self->instrumentArgs,16));
        self->instrumentUpdate();
        self->ui->instrumentDebugLabel->setText("");
        self->midiin.cancelCallback();
        self->midiin.setCallback(&midiHandler,self);
    }
}
