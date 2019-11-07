#include "midihandler.h"

MidiHandler::MidiHandler(int port)
{
    midiin.openPort(port);
    midiin.setCallback(handleMidi, this);
}

MidiHandler::~MidiHandler()
{
    midiin.closePort();
}

void MidiHandler::handleMidi( double timeStamp, std::vector<unsigned char> *message, void *userData )
{
    MidiHandler* self = static_cast<MidiHandler*>(userData);

    for(unsigned int i=0; i<message->size(); i++)
    {
        std::cout << (int)message->at(i) << ":";
    }
}

std::vector<std::string> MidiHandler::getPorts()
{
    std::vector<std::string> ports;

    for(unsigned int i=0; i<midiin.getPortCount(); i++)
    {
        ports.push_back(midiin.getPortName(i));
    }

    return ports;
}

void MidiHandler::changePort(int port)
{
    midiin.closePort();
    midiin.openPort(port);
}
