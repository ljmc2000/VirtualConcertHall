#include "instrumentview.h"
#include "ui_instrumentview.h"

#include <QDebug>
#include <QOpenGLFunctions>

static QString quaver="<svg width=\"280\" height=\"430\">\
        <ellipse cx=\"100\" cy=\"240\" rx=\"80\" ry=\"40\" fill=\"%1\"/>\
        <path d=\"m 170 240 0 -240 c16 72 80 48 80 100\" fill=\"none\" stroke=\"%1\" stroke-width=\"20\" stroke-linecap=\"round\"/>\
    </svg>";

InstrumentView::InstrumentView(InstrumentType type, quint64 args, QWidget *parent) :
    QOpenGLWidget(parent),
    ui(new Ui::InstrumentView),
    noteRenderer(this)
{
    ui->setupUi(this);
    this->instrumentType=type;
    this->instrumentArgs=args;

    screenUpdateTimer.setInterval(33);
    connect(&screenUpdateTimer, SIGNAL(timeout()),
            this, SLOT(update()));
    screenUpdateTimer.start();

    updateInstrument();
}

InstrumentView::~InstrumentView()
{
    delete ui;
}

void InstrumentView::updateInstrument()
{
    switch (instrumentType)
    {
    case PIANO:
        fromPiano();
        break;
    }
}

void InstrumentView::fromPiano()
{
    PianoArgs* args=(PianoArgs*)&instrumentArgs;
    keys.clear();

    quint8 range=args->maxNote-args->minNote;
    float height=size().height()/3;
    float width=size().width()/(1+(7*(range/12.0)));
    QSizeF wKeySize(width,height);
    QSizeF bKeySize(width*.75,height*.5);
    QPointF boffset(width*.75*.5, 0.0);
    QPointF woffset(width,0);

    QPointF position;
    position.setX(0);
    position.setY(size().height()-height);

    for(quint8 i=args->minNote; i<=args->maxNote; i++)
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

void InstrumentView::playNote(quint8 note)
{
    Note n;
    n.note=note;
    notes.append(n);
    update();
}

void InstrumentView::paintGL()
{
    painter.begin(this);

    painter.setPen(Qt::black);
    painter.setBrush(QBrush(Qt::white));

    switch (instrumentType)
    {
        case PIANO:
        {
            PianoArgs *args=(PianoArgs*)&instrumentArgs;

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
                if(n->note>args->maxNote || n->note<args->minNote) continue;

                QRectF dimensions=keys[n->note].dim;
                QPointF h(dimensions.x(),dimensions.y()+(size().height()*.666666666f)*(-n->age/(float)TIMEOUT));
                dimensions.moveTo(h);

                noteRenderer.render(&painter,dimensions);
                if(n->age<TIMEOUT)n->age+=100;
                else notes.removeAt(i);
            }
            break;
        }
    }

    painter.end();
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
