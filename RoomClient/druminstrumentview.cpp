#include "druminstrumentview.h"

#include "roomcommon.h"

using namespace RoomCommon;

DrumInstrumentView::DrumInstrumentView(DrumLayout layout, QWidget *parent):
    InstrumentView(parent)
{
    this->layout=layout;
}

void DrumInstrumentView::updateInstrument()
{
    pads.clear();

    for(PadTemplate temp: presets[layout])
    {
        float sx=size().width(),sy=size().height();
        pads.insert(temp.note,
                    QRectF(
                        QPointF(temp.px*sx,temp.py*sy),
                        QSizeF(temp.rx*sx,temp.ry*sy)
                    )
        );
    }
}

void DrumInstrumentView::paintGL()
{
    painter.begin(this);

    painter.setPen(Qt::black);
    painter.setBrush(QBrush(Qt::red));

    for(const QRectF &pad: pads)
    {
        painter.drawEllipse(pad);
    }

    for(int i=0; i<notes.size(); i++)
    {
        Note *n=&notes[i];
        if(!pads.contains(n->note)) continue;

        QRectF dimensions=pads.value(n->note);
        QPointF h(dimensions.x(),dimensions.y()+(size().height()*.666666666f)*(-n->age/(float)NOTE_TIMEOUT));
        dimensions.moveTo(h);

        noteRenderer.render(&painter,dimensions);
        if(n->age<NOTE_TIMEOUT)n->age+=100;
        else notes.removeAt(i);
    }

    painter.end();
}

PadTemplate::PadTemplate(quint8 note, float px, float py, float rx, float ry)
{
    this->note=note;
    this->px=px;
    this->py=py;
    this->rx=rx;
    this->ry=ry;
}
