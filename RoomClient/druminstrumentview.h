#ifndef DRUMINSTRUMENTVIEW_H
#define DRUMINSTRUMENTVIEW_H

#include <instrumentview.h>

struct PadTemplate
{
    quint8 note;
    float px,py;
    float rx,ry;

    PadTemplate(quint8,float,float,float,float);
};

class DrumInstrumentView : public InstrumentView
{
public:
    DrumInstrumentView(DrumLayout layout, QWidget *parent);
    void updateInstrument();

    static QHash<DrumLayout,QList<PadTemplate>> presets;

protected:
    void paintGL();

private:
    QHash<quint8,QRectF> pads;
    DrumLayout layout;
};

#endif // DRUMINSTRUMENTVIEW_H
