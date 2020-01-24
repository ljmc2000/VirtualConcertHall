#ifndef GUITARUTILS_H
#define GUITARUTILS_H

#include "roomcommon.h"

using namespace RoomCommon;

struct GuitarPrefs
{
    quint8 fretCount=0;
    QList<quint8>strings;

    GuitarPrefs(quint8 fretCount,QList<quint8>strings);
};

class GuitarUtils
{
public:
    static GuitarPrefs & getInstrumentArgs(GuitarTuning tuning);
private:
    static GuitarPrefs standard,dadgad;
};

#endif // GUITARUTILS_H
