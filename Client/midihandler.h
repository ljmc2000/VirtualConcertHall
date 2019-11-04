#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H

#include <rtmidi/RtMidi.h>

class MidiHandler
{
public:
    MidiHandler(int port);
    ~MidiHandler();

private:
    RtMidiIn *midiin;
    RtMidiIn::RtMidiCallback handleMidiFunction = &MidiHandler::handleMidi;
    static void handleMidi( double timeStamp, std::vector<unsigned char> *message, void *userData );
};

#endif // MIDIHANDLER_H
