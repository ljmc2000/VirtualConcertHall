#ifndef DRUMINSTRUMENTVIEW_H
#define DRUMINSTRUMENTVIEW_H

#include <instrumentview.h>

class DrumInstrumentView : public InstrumentView
{
public:
    DrumInstrumentView(DrumLayout layout, QWidget *parent);
    void updateInstrument();

    static QHash<DrumLayout,QHash<quint8,QRectF>> presets;

protected:
    void paintGL();

private:
    QHash<quint8,QRectF> pads;
    DrumLayout layout;
};

#endif // DRUMINSTRUMENTVIEW_H
