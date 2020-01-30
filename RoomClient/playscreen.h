#ifndef PLAYSCREEN_H
#define PLAYSCREEN_H

#include <QNetworkDatagram>
#include <QSettings>
#include <QTimer>
#include <QUdpSocket>
#include <QWidget>

#include <RtMidi.h>

#include "basescreen.h"
#include "httpapiclient.h"
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
    void attemptConnect();
    void closeServer();
    void disconnectFromServer();
    void iterateServertime();
    void quitPlaying();

    void handleDataFromServer();

signals:
    void switchScreen(Mode mode);

private:    //methods
    static void handleMidiIn( double timeStamp, std::vector<unsigned char> *message, void *userData );
    void loadInstrumentConfig(QSettings *prefs);

private:
    Ui::PlayScreen *ui;

    QSettings prefs;
    RtMidiIn midiin;
    HttpAPIClient *httpApiClient;

    QUdpSocket qSocket;
    QHostAddress serverHost;
    quint16 serverPort;
    InstrumentType instrumentType;
    instrument_args_t instrumentArgs;
    QTimer reconnectClock;
    QTimer serverTimeIterator;
    qint64 timestamp;
    quint32 clientId=-1,secretId;
    quint8 reconnectAttempts=MAXCONNECTATTEMPTS;

    bool owner;
};

#endif // PLAYSCREEN_H
