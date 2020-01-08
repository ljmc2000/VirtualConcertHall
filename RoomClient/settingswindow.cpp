#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "mainwindow.h"
#include <iostream>
#include <QObject>

SettingsWindow::SettingsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    setMidiPortsList();

    connect(ui->backButton, SIGNAL(clicked()),
            this, SLOT(returnToLastWindow()));

    connect(ui->midiInputSelector, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setMidiInPort()));

    connect(ui->midiOutputSelector, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setMidiOutPort()));

}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::setMidiPortsList()
{
    ui->midiInputSelector->clear();
    ui->midiOutputSelector->clear();

    int inport=prefs.value("midiInPort").toInt(),outport=prefs.value("midiOutPort").toInt();

    midiin.openPort(inport<midiin.getPortCount() ? inport:0);
    midiin.setCallback(midiHandler,this);
    midiout.openPort(outport<midiin.getPortCount() ? outport:0);

    for(unsigned int i=0; i<midiin.getPortCount(); i++)
    {
        ui->midiInputSelector->addItem(midiin.getPortName(i).c_str());
    }

    for(unsigned int i=0; i<midiout.getPortCount(); i++)
    {
        ui->midiOutputSelector->addItem(midiout.getPortName(i).c_str());
    }

    ui->midiInputSelector->setCurrentIndex(prefs.value("midiInPort").toInt());
    ui->midiOutputSelector->setCurrentIndex(prefs.value("midiOutPort").toInt());

    refreshUsername();
}

void SettingsWindow::setMidiInPort()
{
    int port=ui->midiInputSelector->currentIndex();
    prefs.setValue("midiInPort",port);
    midiin.closePort();
    midiin.openPort(port);
}

void SettingsWindow::setMidiOutPort()
{
    int port=ui->midiOutputSelector->currentIndex();
    prefs.setValue("midiOutPort",port);
    midiout.closePort();
    midiout.openPort(port);
}

void SettingsWindow::returnToLastWindow()
{
    delete this;
}

void SettingsWindow::logout()
{
    prefs.remove("loginToken");
    httpApiClient.signout();
    refreshUsername();
}

void SettingsWindow::login()
{
    MainWindow *w = (MainWindow*)parentWidget();
    w->openLoginWindow();
    delete this;
}

void SettingsWindow::refreshUsername()
{
    QString username = httpApiClient.getUsername();
    if(username != "")
    {
        ui->signinLabel->setText("signed in as "+username);
        ui->signinButton->setText("sign out");
        connect(ui->signinButton, SIGNAL(clicked()),
                this, SLOT(logout()));
    }
    else
    {
        ui->signinLabel->setText("signed out");
        ui->signinButton->setText("sign in");
        connect(ui->signinButton, SIGNAL(clicked()),
                this, SLOT(login()));
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
}
