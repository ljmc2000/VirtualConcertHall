#include "druminstrumentview.h"

#include "roomcommon.h"
#include <QDebug>

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

    qDebug() << pads;
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
        QPointF h(dimensions.x(),dimensions.y()+(size().height()*.666666666f)*(-n->age/(float)TIMEOUT));
        dimensions.moveTo(h);

        noteRenderer.render(&painter,dimensions);
        if(n->age<TIMEOUT)n->age+=100;
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

static QList<PadTemplate> AmmoonDrumPad
({
     PadTemplate(36,
         3.0f/8.0f,
         3.0f/8.0f,
         1.0f/4.0f,
         1.0f/4.0f
     ),

     PadTemplate(38,
         0,
         4.0f/5.0f,
         1.0f/4.0f,
         1.0f/4.0f
     ),

     PadTemplate(42,
         1.0f/8.0f,
         1.0f/2.0f,
         3.0f/16.0f,
         3.0f/16.0f
     ),

     PadTemplate(44,
         7.0f/16.0f,
         11.0f/16.0f,
         1.0f/8.0f,
         1.0f/8.0f
     ),

     PadTemplate(45,
         1-1/4.0f,
         4.0f/5.0f,
         1.0f/4.0f,
         1.0f/4.0f
     ),

     PadTemplate(48,
         9.0f/16.0f,
         4.0f/5.0f,
         3.0f/16.0f,
         3.0f/16.0f
     ),

     PadTemplate(49,
         1.0f/8.0f,
         1.0f/4.0f,
         3.0f/16.0f,
         3.0f/16.0f
     ),

     PadTemplate(50,
         1.0f/4.0f,
         4.0f/5.0f,
         3.0f/16.0f,
         3.0f/16.0f
     ),

     PadTemplate(51,
         11.0f/16.0f,
         1.0f/2.0f,
         3.0f/16.0f,
         3.0f/16.0f
     ),

     PadTemplate(57,
         11.0f/16.0f,
         1.0f/4.0f,
         3.0f/16.0f,
         3.0f/16.0f
     ),
});

QHash<DrumLayout,QList<PadTemplate>> DrumInstrumentView::presets({
    {AMMOON,AmmoonDrumPad},
});
