#include "instrumentview.h"
#include "ui_instrumentview.h"

#include <QDebug>
#include <QOpenGLFunctions>

#include <pianoinstrumentview.h>
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
    switch(type)
    {
        case PIANO:
        {
            PianoArgs *pargs=(PianoArgs*)&args;
            return new PianoInstrumentView(pargs->minNote,pargs->maxNote,parent);
        }

        case DRUM:
        {
            DrumArgs *dargs = (DrumArgs*)&args;
            return new DrumInstrumentView(dargs->layout,parent);
        }
    }
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
