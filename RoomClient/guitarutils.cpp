#include "guitarutils.h"

GuitarPrefs::GuitarPrefs(quint8 fretCount,QList<quint8> strings)
{
    this->fretCount=fretCount;
    this->strings=strings;
}
GuitarPrefs GuitarUtils::standard(18,{40,45,50,55,59,64});
GuitarPrefs GuitarUtils::dadgad(18,{38,45,50,55,57,62});

GuitarPrefs & GuitarUtils::getInstrumentArgs(GuitarTuning tuning)
{
    switch(tuning)
    {
    case STANDARD:
        return standard;
    case DADGAD:
        return dadgad;
    }
}
