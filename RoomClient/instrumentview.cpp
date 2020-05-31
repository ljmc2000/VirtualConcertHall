#include "instrumentview.h"
#include "ui_instrumentview.h"

#include <QDebug>
#include <QOpenGLFunctions>

#include <pianoinstrumentview.h>
#include <guitarinstrumentview.h>
#include <druminstrumentview.h>

static QString quaver="<svg width=\"280\" height=\"430\">\
        <ellipse cx=\"100\" cy=\"240\" rx=\"80\" ry=\"40\" fill=\"%1\"/>\
        <path d=\"m 170 240 0 -240 c16 72 80 48 80 100\" fill=\"none\" stroke=\"%1\" stroke-width=\"20\" stroke-linecap=\"round\"/>\
    </svg>";

InstrumentView::InstrumentView(QWidget *parent) :
    QOpenGLWidget(parent),
    ui(new Ui::InstrumentView),
    noteRenderer(this)
{
    ui->setupUi(this);

    screenUpdateTimer.setInterval(33);
    connect(&screenUpdateTimer, SIGNAL(timeout()),
            this, SLOT(update()));
    screenUpdateTimer.start();
}

InstrumentView::~InstrumentView()
{
    delete ui;
}

InstrumentView* InstrumentView::getInstrumentView(InstrumentType type, instrument_args_t args, QWidget *parent)
{
    InstrumentView *v;

    switch(type)
    {
        case PIANO:
        {
            PianoArgs *pargs=(PianoArgs*)&args;
            v = new PianoInstrumentView(pargs->minNote,pargs->maxNote,parent);
            break;
        }

        case GUITAR:
        {
            GuitarArgs *gargs=(GuitarArgs*)&args;
            v = new GuitarInstrumentView(gargs->tuning,parent);
            break;
        }

        case DRUM:
        {
            DrumArgs *dargs = (DrumArgs*)&args;
            v = new DrumInstrumentView(dargs->layout,parent);
            break;
        }
    }

    v->type=type;
    return v;
}

void InstrumentView::playNote(quint8 note)
{
    Note n;
    n.note=note;
    notes.append(n);
    update();
}

void InstrumentView::initializeGL()
{
    QColor bgcolor = palette().color(QPalette::Window);
    QColor fgcolor = palette().color(QPalette::WindowText);

    glClearColor(bgcolor.redF(),bgcolor.greenF(),bgcolor.blueF(),bgcolor.alphaF());

    noteRenderer.load(quaver
                      .arg(fgcolor.name(QColor::HexRgb))
                      .toUtf8());
}

void InstrumentView::resizeGL(int w, int h)
{
    updateInstrument();
}

void InstrumentView::drawNotes(QHash<quint8,QRectF> *noteSource)
{
    QRectF dimensions;

    for(int i=0; i<notes.size(); i++)
    {
        Note *n=&notes[i];
        if(!noteSource->contains(n->note)) continue;

        dimensions=noteSource->value(n->note);
        QPointF h(dimensions.x(),dimensions.y()+(size().height()*.666666666f)*(-n->age/(float)NOTE_TIMEOUT));
        dimensions.moveTo(h);

        noteRenderer.render(&painter,dimensions);
        if(n->age<NOTE_TIMEOUT)n->age+=100;
        else notes.removeAt(i);
    }
}
