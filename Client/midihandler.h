#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H

#include <rtmidi/RtMidi.h>

class MidiHandler
{
public:
    MidiHandler(int port);
    ~MidiHandler();

    std::vector<std::string> getPorts();
private:
    RtMidiIn midiin;
    static void handleMidi( double timeStamp, std::vector<unsigned char> *message, void *userData );
};

#endif // MIDIHANDLER_H
