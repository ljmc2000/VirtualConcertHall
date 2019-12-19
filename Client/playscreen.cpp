#include "playscreen.h"
#include "ui_playscreen.h"

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

    std::cout << data.data() << "\n";
}
