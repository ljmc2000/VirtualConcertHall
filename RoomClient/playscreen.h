#ifndef PLAYSCREEN_H
#define PLAYSCREEN_H

#include <QNetworkDatagram>
#include <QQueue>
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
    void ping();
    void quitPlaying();

    void handleDataFromServer();

signals:
    void switchScreen(Mode mode);
    void pingChange(int ping);

private:    //methods
    static void handleMidiIn( double timeStamp, std::vector<unsigned char> *message, void *userData );
    void loadInstrumentConfig(QSettings *prefs);
    void sendPacket(char* packet, PacketType packetType);

private:
    Ui::PlayScreen *ui;

    QSettings prefs;
    RtMidiIn midiin;
    HttpAPIClient *httpApiClient;

    QUdpSocket qSocket;
    QHostAddress serverHost;
    quint16 serverPort;
    room_id_t roomId;
    InstrumentType instrumentType;
    instrument_args_t instrumentArgs;
    QTimer reconnectClock;
    QTimer pingOffsetSyncClock;
    qint16 timeOffset=0,pingOffset=0;
    client_id_t clientId=-1,secretId;
    quint8 reconnectAttempts=MAXCONNECTATTEMPTS;

    bool owner;
};

extern void givePlayerUsername(client_id_t id, Ui::PlayScreen *ps, HttpAPIClient *httpApiClient);

#endif // PLAYSCREEN_H
