#include "druminstrumentview.h"

#include "roomcommon.h"
#include <iostream>

using namespace RoomCommon;

DrumInstrumentView::DrumInstrumentView(DrumLayout layout, QWidget *parent):
    InstrumentView(parent)
{
    this->layout=layout;
}

void DrumInstrumentView::updateInstrument()
{
    this->pads=presets[layout];

    for(QRectF &pad: pads)
    {
        pad.setX(pad.x() * size().width());
        pad.setY(pad.y() * size().height());
        //pad.setSize(QSizeF(pad.width() * size().width(),pad.height() * size().height()));

        std::cout << pad.x() << ' ' << pad.y()<< ' ' << pad.width()<< ' ' << pad.height() << '\n';
    }
}

void DrumInstrumentView::paintGL()
{
    painter.begin(this);

    painter.setPen(Qt::black);
    painter.setBrush(QBrush(Qt::red));

    for(QRectF pad: pads)
    {
        painter.drawEllipse(pad);
    }

    for(int i=notes.size()-1; i>=0; i--)
    {
        Note *n=&notes[i];
        QRectF dimensions=pads.value(n->note);

        noteRenderer.render(&painter,dimensions);
        if(n->age<TIMEOUT)n->age+=100;
        else notes.removeAt(i);
    }

    painter.end();
}

static QHash<quint8,QRectF> AmmoonDrumPad
{
    {38,QRectF(QPointF(4.0f/5.0f,1.0f/4.0f),
               QSizeF(1.0f/4.0f,1.0f/4.0f))},
};

QHash<DrumLayout,QHash<quint8,QRectF>> DrumInstrumentView::presets(
{
    {AMMOON,AmmoonDrumPad},
});
