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
            k.color=Qt::black;
            k.dim=QRectF(position-boffset,bKeySize);
            break;
        default:
            k.color=Qt::white;
            k.dim=QRectF(position,wKeySize);
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
            painter.drawRect(k.dim);
    }

    painter.setBrush(QBrush(Qt::black));
    for(PianoKey k: keys)
    {
        if(k.color==Qt::black)
            painter.drawRect(k.dim);
    }

    for(int i=notes.size()-1; i>=0; i--)
    {
        Note *n=&notes[i];
        if(n->note>maxNote || n->note<minNote) continue;

        QRectF dimensions=keys[n->note].dim;
        QPointF h(dimensions.x(),dimensions.y()+(size().height()*.666666666f)*(-n->age/(float)NOTE_TIMEOUT));
        dimensions.moveTo(h);

        noteRenderer.render(&painter,dimensions);
        if(n->age<NOTE_TIMEOUT)n->age+=100;
        else notes.removeAt(i);
    }

    painter.end();
}
