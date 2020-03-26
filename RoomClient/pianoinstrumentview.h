#ifndef PIANOVIEW_H
#define PIANOVIEW_H

#include "instrumentview.h"

struct PianoKey: public QRectF
{
    QColor color;
    PianoKey& operator=(QRectF rect){QRectF::operator=(rect);return *this;};
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
