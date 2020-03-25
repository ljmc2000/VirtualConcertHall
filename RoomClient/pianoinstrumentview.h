#ifndef PIANOVIEW_H
#define PIANOVIEW_H

#include "instrumentview.h"

struct PianoKey
{
    QRectF dim;
    QColor color;
};

class PianoInstrumentView : public InstrumentView
{
public:
    PianoInstrumentView(quint8 minNote, quint8 maxNote, QWidget *parent = nullptr);
    void updateInstrument();

protected:
    void paintGL();

private:
    QHash<quint8,PianoKey> keys;
    int minNote,maxNote;
};

#endif // PIANOVIEW_H
