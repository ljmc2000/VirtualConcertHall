#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H

#include <RtMidi.h>
#include <QObject>

class MidiHandler: public QObject
{
    Q_OBJECT

public:
    MidiHandler(std::string name);
    ~MidiHandler();

    std::vector<std::string> getPorts();

public slots:
    void changePort(int port);

private:
    RtMidiIn midiin;
    static void handleMidi( double timeStamp, std::vector<unsigned char> *message, void *userData );
};

#endif // MIDIHANDLER_H
