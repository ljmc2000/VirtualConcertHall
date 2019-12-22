#ifndef PLAYSCREEN_H
#define PLAYSCREEN_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>

#include <QGraphicsSvgItem>
#include <QGraphicsScene>
#include <midihandler.h>

namespace Ui {
class PlayScreen;
}

class PlayScreen : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlayScreen(QWidget *parent = nullptr);
    ~PlayScreen();

    void setLastWindow(QMainWindow *w);
    void showEvent(QShowEvent *e);

private:
    Ui::PlayScreen *ui;

    QMainWindow *lastWindow;

    QGraphicsSvgItem instramentVisual;
    QGraphicsScene scene;

    MidiHandler midiHandler;
};

#endif // PLAYSCREEN_H
