#include "playscreen.h"
#include "ui_playscreen.h"

PlayScreen::PlayScreen(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlayScreen),
    instramentVisual("piano.svg"),
    midiHandler(&qSocket)
{
    ui->setupUi(this);

    scene.addItem(&instramentVisual);
    ui->instrament->setScene(&scene);
    ui->instrament->setSceneRect(scene.sceneRect());
    ui->instrament->show();
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
