#include "playscreen.h"
#include "ui_playscreen.h"
#include "roomcommon.h"

using namespace RoomCommon;

PlayScreen::PlayScreen(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlayScreen),
    instramentVisual("piano.svg")
{
    ui->setupUi(this);

    QSettings prefs;

    scene.addItem(&instramentVisual);
    ui->instrament->setScene(&scene);
    ui->instrament->setSceneRect(scene.sceneRect());
    ui->instrament->show();
}

PlayScreen::~PlayScreen()
{
    delete ui;
}

void PlayScreen::showEvent(QShowEvent *event)
{
    ui->instrament->fitInView(scene.sceneRect(), Qt::KeepAspectRatio);
}
