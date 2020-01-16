#ifndef PLAYSCREEN_H
#define PLAYSCREEN_H

#include "basescreen.h"
#include "httpapiclient.h"
#include <QWidget>
#include <QUdpSocket>
#include <QTimer>

#include <QGraphicsSvgItem>
#include <QGraphicsScene>
#include <midihandler.h>

namespace Ui {
class PlayScreen;
}

class PlayScreen : public QWidget
{
    Q_OBJECT

public:
    explicit PlayScreen(RoomConnectionInfo r, HttpAPIClient *httpApiClient, QWidget *parent = nullptr);
    ~PlayScreen();

    void showEvent(QShowEvent *e);
    void resizeEvent(QResizeEvent *event);

private slots:
    void askQuit();
    void quitPlaying();

signals:
    void switchScreen(Mode mode);

private:
    Ui::PlayScreen *ui;

    QGraphicsSvgItem *instramentVisual;
    QGraphicsScene scene;

    MidiHandler *midiHandler;
    HttpAPIClient *httpApiClient;

    bool owner;
};

#endif // PLAYSCREEN_H
