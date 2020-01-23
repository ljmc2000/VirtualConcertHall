#include "instrumentview.h"
#include "ui_instrumentview.h"

#include <QDebug>
#include <QOpenGLFunctions>

static QString note="<svg width=\"360\" height=\"480\">\
        <g transform=\"translate(15 -500), scale(3,3)\">\
         <ellipse cx=\"30.069\" cy=\"277.61\" rx=\"27.933\" ry=\"18.536\" style=\"paint-order:normal\" fill=\"rgb(%1,%2,%3)\"/>\
         <path d=\"m54.026 276.84-4.175-101.78c7.9922 36.668 37.012 24.436 37.973 53.744\" fill=\"none\" stroke=\"rgb(%1,%2,%3)\" stroke-dashoffset=\"200.02\" stroke-linecap=\"round\" stroke-linejoin=\"round\" stroke-width=\"8.3981\" style=\"paint-order:normal\"/>\
        </g>\
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

void InstrumentView::fromPiano(quint8 minNote, quint8 maxNote)
{
    this->minNote=minNote;
    this->maxNote=maxNote;
    fromPiano();
}

void InstrumentView::fromPiano()
{
    keys.clear();

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

void InstrumentView::playNote(quint8 note)
{
    if(note<=maxNote && note>=minNote)
    {
        Note n;
        n.note=note;
        notes.append(n);
        update();
    }
}

void InstrumentView::paintGL()
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
        QRectF dimensions=keys[n->note].dim;
        QPointF h(dimensions.x(),dimensions.y()+(size().height()*.666666666f)*(-n->age/(float)TIMEOUT));
        dimensions.moveTo(h);

        noteRenderer.render(&painter,dimensions);
        if(n->age<TIMEOUT)n->age+=100;
        else notes.removeAt(i);
    }

    painter.end();
}

void InstrumentView::initializeGL()
{
    QColor bgcolor = palette().color(QPalette::Window);
    QColor fgcolor = palette().color(QPalette::WindowText);

    glClearColor(bgcolor.redF(),bgcolor.greenF(),bgcolor.blueF(),bgcolor.alphaF());

    noteRenderer.load(note
                      .arg((int)255*fgcolor.redF())
                      .arg((int)255*fgcolor.greenF())
                      .arg((int)255*fgcolor.blueF())
                      .toUtf8());
}

void InstrumentView::resizeGL(int w, int h)
{
    fromPiano();
}
