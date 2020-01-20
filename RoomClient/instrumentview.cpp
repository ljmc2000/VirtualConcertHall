#include "instrumentview.h"
#include "ui_instrumentview.h"

QString InstrumentView::note="<rect %1 width=\"%2\" height=\"%3\" x=\"%5\" y=\"%4\"/>";

InstrumentView::InstrumentView(QWidget *parent) :
    QSvgWidget(parent),
    ui(new Ui::InstrumentView)
{
    ui->setupUi(this);
}

InstrumentView::~InstrumentView()
{
    delete ui;
}

void InstrumentView::fromPiano(quint8 minNote, quint8 maxNote)
{
    QString svg="<svg width=\"%1\" height=\"%2\"> %3 %4 </svg>";
    quint8 range=maxNote-minNote;
    double height=(VIEWHEIGHT/3);
    double width=VIEWWIDTH/(1+(7*(range/12.0)));
    double boffset=width*.75*.5;
    svg=svg.arg(VIEWWIDTH).arg(VIEWHEIGHT);

    QString blackNotes="",blackNote=note.arg("fill=\"black\"").arg(width*0.75).arg(height/2).arg(VIEWHEIGHT-(height));
    QString whiteNotes="",whiteNote=note.arg("fill=\"white\" stroke=\"black\" stroke-width=\"1\"").arg(width).arg(height).arg(VIEWHEIGHT-height);

    double prog=0;
    for(quint8 i=minNote; i<=maxNote; i++)
    {
        switch(i%12)
        {
        case 1:
        case 3:
        case 6:
        case 8:
        case 10:
            blackNotes.append(blackNote.arg(prog-boffset));
            break;
        default:
            whiteNotes.append(whiteNote.arg(prog));
            prog += width;
            break;
        }
    }

    svg=svg.arg(whiteNotes).arg(blackNotes);
    load(svg.toUtf8());
}
