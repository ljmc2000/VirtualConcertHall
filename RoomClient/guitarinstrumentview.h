#ifndef GUITARINSTRUMENTVIEW_H
#define GUITARINSTRUMENTVIEW_H

#include "instrumentview.h"

class GuitarInstrumentView : public InstrumentView
{
public:
    GuitarInstrumentView(GuitarTuning tuning, QWidget *parent = nullptr);
    void updateInstrument();

protected:
    void paintGL();

private:
    GuitarTuning tuning;
    QHash<quint8,QRectF> noteSource;
    QList<QLineF> strings,frets;
    QSvgRenderer guitarRenderer;

    static QColor stringColor, fretColor;
    static QString guitarsvg;
};

#endif // GUITARINSTRUMENTVIEW_H
