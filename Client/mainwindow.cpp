#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QObject>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    midiHandler = new MidiHandler(&qSocket,"127.0.0.1",1998);

    comboBox = this->findChild<QComboBox*>("comboBox");
    setPorts(midiHandler->getPorts());
    QObject::connect(
                comboBox, SIGNAL(currentIndexChanged(int)),
                midiHandler, SLOT(changePort(int)));

    serverIPBox = this->findChild<QLineEdit*>("serverIPBox");
    connectButton = this->findChild<QPushButton*>("connectButton");
    QObject::connect(
                connectButton, SIGNAL(clicked()),
                this, SLOT(setAddress())
                );
}

MainWindow::~MainWindow()
{
    delete ui;
    delete midiHandler;
}

void MainWindow::setPorts(std::vector<std::string> ports)
{
    for(unsigned int i=0; i<ports.size(); i++)
    {
        comboBox->addItem(ports.at(i).c_str());
    }
}

void MainWindow::setAddress()
{
    midiHandler->setAddress(serverIPBox->text());
}
