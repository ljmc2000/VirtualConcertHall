#include "instrumentview.h"
#include "ui_instrumentview.h"
#include <QDebug>
#include <QOpenGLFunctions>

static QString note="<svg width=\"360\" height=\"480\">\
        <g transform=\"translate(0 -170)\">\
         <ellipse cx=\"30.069\" cy=\"277.61\" rx=\"27.933\" ry=\"18.536\" style=\"paint-order:normal\" fill=\"rgb(%1,%2,%3)\"/>\
         <path d=\"m54.026 276.84-4.175-101.78c7.9922 36.668 37.012 24.436 37.973 53.744\" fill=\"none\" stroke=\"rgb(%1,%2,%3)\" stroke-dashoffset=\"200.02\" stroke-linecap=\"round\" stroke-linejoin=\"round\" stroke-width=\"8.3981\" style=\"paint-order:normal\"/>\
        </g>\
       </svg>";
static QString key="<rect %1 width=\"%2\" height=\"%3\" x=\"%4\" y=\"%5\"/>";

InstrumentView::InstrumentView(QWidget *parent) :
    QOpenGLWidget(parent),
    ui(new Ui::InstrumentView),
    painter(this),
    instrumentRenderer(this),
    noteRenderer(this)
{
    ui->setupUi(this);
}

InstrumentView::~InstrumentView()
{
    delete ui;
}

void InstrumentView::fromPiano(quint8 minNote, quint8 maxNote)
{
    noteSource.clear();

    QString svg="<svg width=\"%1\" height=\"%2\"> %3 %4 </svg>";
    quint8 range=maxNote-minNote;
    double height=(VIEWHEIGHT/3);
    double width=VIEWWIDTH/(1+(7*(range/12.0)));
    QPointF boffset(width*.75*.5, 0.0);
    QPointF woffset(width,0);
    svg=svg.arg(VIEWWIDTH).arg(VIEWHEIGHT);
    noteSize=QSizeF(width,width);

    QString blackNotes="",blackNote=key.arg("fill=\"black\"").arg(width*0.75).arg(height/2);
    QString whiteNotes="",whiteNote=key.arg("fill=\"white\" stroke=\"black\" stroke-width=\"1\"").arg(width).arg(height);

    QPointF position;
    position.setX(0);
    position.setY(VIEWHEIGHT-height);

    for(quint8 i=minNote; i<=maxNote; i++)
    {
        QPointF pos=position;

        switch(i%12)
        {
        case 1:
        case 3:
        case 6:
        case 8:
        case 10:
            pos-=boffset;
            blackNotes.append(blackNote.arg(pos.x()).arg(pos.y()));
            break;
        default:
            whiteNotes.append(whiteNote.arg(pos.x()).arg(pos.y()));
            position += woffset;
            break;
        }

        noteSource.insert(i,pos);
    }

    svg=svg.arg(whiteNotes).arg(blackNotes);
    instrumentRenderer.load(svg.toUtf8());
    update();
}

void InstrumentView::playNote(quint8 note)
{
    notes.append(noteSource[note]);
    update();
}

void InstrumentView::paintEvent(QPaintEvent *e)
{
    noteSize.scale(this->size(),Qt::KeepAspectRatio);
    painter.begin(this);
    instrumentRenderer.render(&painter);

    for(QPointF pos: notes)
    {
        QRectF bounds(pos,noteSize);
        noteRenderer.render(&painter,bounds);
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
