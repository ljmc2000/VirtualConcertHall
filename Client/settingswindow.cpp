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
            this, SLOT(setMidiPort()));

    connect(ui->confirmButton, SIGNAL(clicked()),
            this, SLOT(setAddress()));

}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::setLastWindow(QMainWindow *w)
{
    lastWindow = w;
}

void SettingsWindow::setMidiPortsList()
{
    ui->midiInputSelector->clear();

    for(unsigned int i=0; i<midiin.getPortCount(); i++)
    {
        ui->midiInputSelector->addItem(midiin.getPortName(i).c_str());
    }

    ui->midiInputSelector->setCurrentIndex(prefs.value("midiPort").toInt());
}

void SettingsWindow::setAddress()
{
    prefs.setValue("serverHost",ui->serverIPBox->text());
    prefs.setValue("serverPort",ui->serverPortBox->text());
}

void SettingsWindow::setMidiPort()
{
    prefs.setValue("midiPort",ui->midiInputSelector->currentIndex());
}

void SettingsWindow::returnToLastWindow()
{
    this->hide();
    lastWindow->show();
}
