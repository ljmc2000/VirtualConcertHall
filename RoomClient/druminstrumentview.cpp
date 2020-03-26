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

    drawNotes(&pads);

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
