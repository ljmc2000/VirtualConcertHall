#ifndef REPLAYPLAYER_H
#define REPLAYPLAYER_H

#include "ui_replayplayer.h"
#include "basescreen.h"
#include "fluidsynth.h"
#include "replay.h"
#include "roomcommon.h"

#include <QFile>
#include <QThread>

namespace Ui {
class ReplayPlayer;
}

class ReplayPlayerThread: public QThread
{
public:
    ReplayPlayerThread(QString filename, QString soundfont, QObject *parent=nullptr);
    ~ReplayPlayerThread();
    void run();

private: //methods
    void playNote(ReplayLogChunk chunk);

private: //attrs
    QFile in;
    ReplayLogChunk chunk;
    fluid_synth_t *synth;
    fluid_audio_driver_t *driver;
};

void playReplay(QString filename, bool *stopflag);

class ReplayPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit ReplayPlayer(QWidget *parent = nullptr);
    ~ReplayPlayer();

signals:
    void switchScreen(Mode mode);

private slots:
    void selectReplay();
    void play();
    void stop();

private:
    Ui::ReplayPlayer *ui;
    ReplayPlayerThread *t=nullptr;
    QString soundfont;
};

#endif // REPLAYPLAYER_H
