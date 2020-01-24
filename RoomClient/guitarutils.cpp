#include "guitarutils.h"

GuitarUtils::GuitarUtils()
{
    dadgad.lowestNote=38;
    dadgad.gaps1=0x75;
    dadgad.gaps2=0x52;
    dadgad.gaps3=0x50;
}

quint64 GuitarUtils::getInstrumentArgs(GuitarTuning tuning)
{
    switch(tuning)
    {
    case EADGBE:
        return *((quint64*)&eadgbe);
    case DADGAD:
        return *((quint64*)&dadgad);
    }
}
