#include "instrumentview.h"
#include "ui_instrumentview.h"
#include <QDebug>
#include <QOpenGLFunctions>

QString InstrumentView::note="<rect %1 width=\"%2\" height=\"%3\" x=\"%4\" y=\"%5\"/>";

InstrumentView::InstrumentView(QWidget *parent) :
    QOpenGLWidget(parent),
    ui(new Ui::InstrumentView),
    painter(this),
    renderer(this)
    //quaver("quaver.svg")
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
    double boffset=width*.75*.5;
    svg=svg.arg(VIEWWIDTH).arg(VIEWHEIGHT);

    QString blackNotes="",blackNote=note.arg("fill=\"black\"").arg(width*0.75).arg(height/2);
    QString whiteNotes="",whiteNote=note.arg("fill=\"white\" stroke=\"black\" stroke-width=\"1\"").arg(width).arg(height);

    Position position;
    position.x=0;
    position.y=VIEWHEIGHT-(height);

    for(quint8 i=minNote; i<=maxNote; i++)
    {
        Position pos=position;

        switch(i%12)
        {
        case 1:
        case 3:
        case 6:
        case 8:
        case 10:
            pos.x-=boffset;
            blackNotes.append(blackNote.arg(pos.x).arg(pos.y));
            break;
        default:
            whiteNotes.append(whiteNote.arg(pos.x).arg(pos.y));
            position.x += width;
            break;
        }

        noteSource.insert(i,pos);
    }

    svg=svg.arg(whiteNotes).arg(blackNotes);
    renderer.load(svg.toUtf8());
    update();
}

void InstrumentView::playNote(quint8 note)
{
    Position *p = &noteSource[note];
}

void InstrumentView::paintEvent(QPaintEvent *e)
{
    painter.begin(this);
    renderer.render(&painter);
    painter.end();
}
