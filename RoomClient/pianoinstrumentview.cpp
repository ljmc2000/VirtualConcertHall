#include "pianoinstrumentview.h"

PianoInstrumentView::PianoInstrumentView(quint8 minNote, quint8 maxNote, QWidget *parent):
    InstrumentView(parent)
{
    this->minNote=minNote;
    this->maxNote=maxNote;
}

void PianoInstrumentView::updateInstrument()
{
    quint8 range=maxNote-minNote;
    float height=size().height()/3;
    float width=size().width()/(1+(7*(range/12.0)));
    QSizeF wKeySize(width,height);
    QSizeF bKeySize(width*.75,height*.5);
    QPointF boffset(width*.75*.5, 0.0);
    QPointF woffset(width,0);

    QPointF position;
    position.setX(0);
    position.setY(size().height()-height);

    keys.clear();
    for(quint8 i=minNote; i<=maxNote; i++)
    {
        PianoKey k;

        switch(i%12)
        {
        case 1:
        case 3:
        case 6:
        case 8:
        case 10:
            k=QRectF(position-boffset,bKeySize);
            k.color=Qt::black;
            break;
        default:
            k=QRectF(position,wKeySize);
            k.color=Qt::white;
            position += woffset;
            break;
        }

        keys.insert(i,k);
    }

    update();
}

void PianoInstrumentView::paintGL()
{
    painter.begin(this);

    painter.setPen(Qt::black);

    painter.setBrush(QBrush(Qt::white));
    for(PianoKey k: keys)
    {
        if(k.color==Qt::white)
            painter.drawRect(k);
    }

    painter.setBrush(QBrush(Qt::black));
    for(PianoKey k: keys)
    {
        if(k.color==Qt::black)
            painter.drawRect(k);
    }

    drawNotes((QHash<quint8,QRectF>*)&keys);

    painter.end();
}
