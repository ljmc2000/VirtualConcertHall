#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QObject>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    midiHandler = new MidiHandler("VirtualConcertHallClient");

    comboBox = this->findChild<QComboBox*>("comboBox");
    setPorts(midiHandler->getPorts());
    QObject::connect(
                comboBox, SIGNAL(currentIndexChanged(int)),
                midiHandler, SLOT(changePort(int)));
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
