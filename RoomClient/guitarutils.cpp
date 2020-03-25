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

void GuitarUtils::getFretDistances(int numFrets, float scale, QList<float> *distances)
//based on https://buildyourguitar.com/resources/tips/fretdist.htm C-program for calculating fret
//distances for any scale length Written by Chris St. Pierre
{
    float distance=0, scaling_factor=0, location;
    distances->clear();

    for (int fret = 1; fret <= numFrets; fret++)
    {
       location = scale - distance;
       scaling_factor = location / 17.817;
       distance += scaling_factor;
       distances->append(distance);
    }
}
