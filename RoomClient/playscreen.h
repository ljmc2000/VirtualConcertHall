#ifndef PLAYSCREEN_H
#define PLAYSCREEN_H

#include <QGraphicsSvgItem>
#include <QTimer>
#include <QUdpSocket>
#include <QWidget>

#include "basescreen.h"
#include "httpapiclient.h"
#include "instrumentview.h"
#include "midihandler.h"
#include "roomcommon.h"

using namespace RoomCommon;

namespace Ui {
class PlayScreen;
}

class PlayScreen : public QWidget
{
    Q_OBJECT

public:
    explicit PlayScreen(RoomConnectionInfo r, HttpAPIClient *httpApiClient, QWidget *parent = nullptr);
    ~PlayScreen();

private slots:
    void askQuit();
    void quitPlaying();

    void addInstrumentView(quint32 clientId, InstrumentType instrament, quint64 args);
    void removeInstrumentView(quint32 clientId);
    void handleMidiPacket(quint32 clientId, quint8* message);

signals:
    void switchScreen(Mode mode);

private:
    Ui::PlayScreen *ui;

    QHash<quint32,InstrumentView*> instrumentViews;

    MidiHandler *midiHandler;
    HttpAPIClient *httpApiClient;

    bool owner;
};

#endif // PLAYSCREEN_H
