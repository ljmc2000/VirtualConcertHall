#include "midihandler.h"


MidiHandler::MidiHandler(int port)
{
    midiin = new RtMidiIn();
    midiin->openPort(port);
    midiin->setCallback(handleMidi);
}

MidiHandler::~MidiHandler()
{
}

void MidiHandler::handleMidi( double timeStamp, std::vector<unsigned char> *message, void *userData )
{
    for(int i=0; i<message->size(); i++)
    {
        std::cout << (int)message->at(i) << ":";
    }

    std::cout << "\n";
}
