#ifndef GUITARUTILS_H
#define GUITARUTILS_H

#include "roomcommon.h"

using namespace RoomCommon;

class GuitarUtils
{
public:
    GuitarUtils();
    quint64 getInstrumentArgs(GuitarTuning tuning);
private:
    GuitarArgs eadgbe,dadgad;
};

#endif // GUITARUTILS_H
