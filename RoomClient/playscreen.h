#ifndef PLAYSCREEN_H
#define PLAYSCREEN_H

#include "basescreen.h"
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
    explicit PlayScreen(quint32 secretId,QString ipaddress,quint16 port, QWidget *parent = nullptr);
    ~PlayScreen();

    void showEvent(QShowEvent *e);

signals:
    void switchScreen(Mode mode);

private:
    Ui::PlayScreen *ui;

    QGraphicsSvgItem instramentVisual;
    QGraphicsScene scene;

    MidiHandler midiHandler;
};

#endif // PLAYSCREEN_H
