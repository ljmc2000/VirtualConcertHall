#include "playscreen.h"
#include "ui_playscreen.h"
#include "roomcommon.h"

#include <QMessageBox>

using namespace RoomCommon;

PlayScreen::PlayScreen(RoomConnectionInfo r,HttpAPIClient *httpApiClient,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayScreen)
{
    ui->setupUi(this);
    this->httpApiClient=httpApiClient;
    this->owner=r.owner;

    this->midiHandler=new MidiHandler(r.secretId,r.roomIp,r.roomPort,parent);
    connect(midiHandler, SIGNAL(destroyed()),
            this, SLOT(quitPlaying()));

    connect(midiHandler, &MidiHandler::playerJoin,
            this, &PlayScreen::addInstrumentView);

    connect(ui->exitButton, SIGNAL(clicked()),
            this, SLOT(askQuit()));
}

PlayScreen::~PlayScreen()
{
    for(InstrumentView *v: instrumentViews)
        delete v;

    midiHandler->deleteLater();
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
        midiHandler->closeServer();
    }

    qDebug() << "Disconnected from server";
    httpApiClient->refreshPlayerState();
    emit switchScreen(MAINMENU);
}

void PlayScreen::addInstrumentView(quint32 clientId, InstrumentType instrament, quint64 instrumentArgs)
{
    InstrumentView *v;
    quint8 *args=(quint8*)&instrumentArgs;

    if(!instrumentViews.contains(clientId)) {
        v=new InstrumentView(ui->playArea);
    } else {
        v=instrumentViews[clientId];
    }

    switch (instrament)
    {
    case PIANO:
        v->fromPiano(args[0],args[1]);
        break;
    }

    instrumentViews.insert(clientId,v);
    v->show();
}

void PlayScreen::removeInstrumentView()
{

}
