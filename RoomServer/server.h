#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>

#include <RtMidi.h>

class Server: public QObject
{
    Q_OBJECT;

public:
    Server(int port);
    ~Server();

public slots:
    void readPendingDatagrams();

private:
    QUdpSocket qSocket;
    RtMidiOut midiout;
};

#endif // SERVER_H
