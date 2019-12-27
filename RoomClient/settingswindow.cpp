#include "settingswindow.h"
#include "ui_settingswindow.h"
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

    connect(ui->confirmButton, SIGNAL(clicked()),
            this, SLOT(setAddress()));

}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::setMidiPortsList()
{
    ui->midiInputSelector->clear();

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
}

void SettingsWindow::setAddress()
{
    prefs.setValue("serverHost",ui->serverIPBox->text());
    prefs.setValue("serverPort",ui->serverPortBox->text());
}

void SettingsWindow::setMidiInPort()
{
    prefs.setValue("midiInPort",ui->midiInputSelector->currentIndex());
}

void SettingsWindow::setMidiOutPort()
{
    prefs.setValue("midiOutPort",ui->midiOutputSelector->currentIndex());
}

void SettingsWindow::returnToLastWindow()
{
    delete this;
}
