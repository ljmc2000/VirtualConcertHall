#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H

#include <RtMidi.h>
#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>

class MidiHandler: public QObject
{
    Q_OBJECT

public:
    MidiHandler(QUdpSocket *qSocket, std::string server, int port);
    ~MidiHandler();

    std::vector<std::string> getPorts();
    void setAddress(QString address);

public slots:
    void changePort(int port);

private:
    RtMidiIn midiin;
    QUdpSocket *qSocket;
    QHostAddress server;

    unsigned int serverPort;
    static void handleMidi( double timeStamp, std::vector<unsigned char> *message, void *userData );
};

#endif // MIDIHANDLER_H
