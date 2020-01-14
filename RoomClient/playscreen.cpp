#include "playscreen.h"
#include "ui_playscreen.h"
#include "roomcommon.h"

#include <QMessageBox>

using namespace RoomCommon;

PlayScreen::PlayScreen(RoomConnectionInfo r,HttpAPIClient *httpApiClient,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayScreen),
    midiHandler(r.secretId,r.roomIp,r.roomPort,parent)
{
    ui->setupUi(this);
    this->httpApiClient=httpApiClient;
    this->owner=r.owner;

    connect(&midiHandler, SIGNAL(disconnectedFromServer()),
            this, SLOT(quitPlaying()));

    connect(ui->exitButton, SIGNAL(clicked()),
            this, SLOT(askQuit()));


    instramentVisual=new QGraphicsSvgItem("piano.svg");
    scene.addItem(instramentVisual);
    ui->instrament->setScene(&scene);
    ui->instrament->setSceneRect(scene.sceneRect());
    ui->instrament->show();
}

PlayScreen::~PlayScreen()
{
    delete instramentVisual;
    delete ui;
}

void PlayScreen::askQuit()
{
    QMessageBox::StandardButton reply=QMessageBox::question(this, "Leave?", owner ?
                                                                "Are you sure you wish to close the room?":
                                                                "Are you sure you wish to leave the room?",
                                                            QMessageBox::Yes|QMessageBox::No);
    switch (reply)
    {
    case QMessageBox::Yes:
        quitPlaying();
        break;
    default:
        break;
    }
}

void PlayScreen::quitPlaying()
{
    if(!owner)
    {
        httpApiClient->leaveRoom();
    }
    else
    {
        httpApiClient->closeRoom();
    }

    emit switchScreen(MAINMENU);
}

void PlayScreen::showEvent(QShowEvent *event)
{
    ui->instrament->fitInView(scene.sceneRect(), Qt::KeepAspectRatio);
}

void PlayScreen::resizeEvent(QResizeEvent *event)
{
    ui->instrament->fitInView(scene.sceneRect(), Qt::KeepAspectRatio);
}
