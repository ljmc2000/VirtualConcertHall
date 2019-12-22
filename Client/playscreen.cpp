#include "playscreen.h"
#include "ui_playscreen.h"
#include "roomcommon.h"

using namespace RoomCommon;

PlayScreen::PlayScreen(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlayScreen),
    instramentVisual("piano.svg"),
    midiHandler(&qSocket)
{
    ui->setupUi(this);

    QSettings prefs;

    scene.addItem(&instramentVisual);
    ui->instrament->setScene(&scene);
    ui->instrament->setSceneRect(scene.sceneRect());
    ui->instrament->show();

    qSocket.connectToHost(QHostAddress(prefs.value("serverHost").toString()),prefs.value("serverPort").toInt());
    connect(
                &qSocket, SIGNAL(readyRead()),
                this, SLOT(handleDataFromServer())
            );

    heartBeatClock.setInterval(HEARTBEATINTERVAL);
    connect(
                &heartBeatClock, SIGNAL(timeout()),
                this, SLOT(heartBeat())
            );
    heartBeatClock.start();
}

PlayScreen::~PlayScreen()
{
    delete ui;
}

void PlayScreen::setLastWindow(QMainWindow *w)
{
    lastWindow = w;
}

void PlayScreen::showEvent(QShowEvent *event)
{
    ui->instrament->fitInView(scene.sceneRect(), Qt::KeepAspectRatio);
}

void PlayScreen::handleDataFromServer()
{
    //deal with server stuff here
    QNetworkDatagram datagram = qSocket.receiveDatagram();
    QByteArray data = datagram.data();

    foreach(unsigned char c, data)
    {
        std::cout << (int) c << ":";
    }
    std::cout << "\n";
}

void PlayScreen::heartBeat()
{
    QByteArray data;
    data.push_front(HEARTBEAT);

    qSocket.writeDatagram(QNetworkDatagram(data));
}
