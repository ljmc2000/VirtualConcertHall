#include "guitarinstrumentview.h"
#include "guitarutils.h"
#include <QDebug>

#define FRETBOARDWIDTHPERCENT .05f
QColor GuitarInstrumentView::stringColor(24,100,50);
QColor GuitarInstrumentView::fretColor(0,0,63);

QString GuitarInstrumentView::guitarsvg("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
                                        <svg width=\"100\" height=\"100\" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:cc=\"http://creativecommons.org/ns#\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\
                                         <path d=\"m99.432 43.979c-0.43124-12.186 0.19992-23.975-16.022-27.034-19.857 17.612-39.915 26.958-60.843 0.43958-24.27 21.106-31.664 43.675-2.0281 69.455 25.421-27.19 44.857-21.141 61.655-0.43959 16.427-7.1117 16.628-17.637 17.239-27.035z\" fill=\"#aba9a6\" fill-rule=\"evenodd\" stroke=\"#000\" stroke-width=\".73896\"/>\
                                         <ellipse cx=\"51.388\" cy=\"52.247\" rx=\"9.8217\" ry=\"9.5306\" fill=\"#835500\" stroke=\"#000\" stroke-dashoffset=\"200.02\" stroke-linecap=\"round\" stroke-linejoin=\"round\" stroke-width=\".79993\" style=\"paint-order:normal\"/>\
                                        </svg>");

GuitarInstrumentView::GuitarInstrumentView(GuitarTuning tuning, QWidget *parent):
    InstrumentView(parent)
{
    this->tuning=tuning;

    guitarRenderer.load(guitarsvg.toUtf8());
}

void GuitarInstrumentView::updateInstrument()
{
    GuitarPrefs prefs = GuitarUtils::getInstrumentArgs(tuning);
    QList<float> fretDistances;
    float scale = size().width();
    float fretboard_top,fretboard_bottom;
    float middle=size().height()/2;
    float fretboard_width_percent=(1+prefs.strings.size())/100.0f;
    float width_twixt_strings;

    fretboard_bottom=middle-(middle*fretboard_width_percent);
    fretboard_top=middle+(middle*fretboard_width_percent);
    width_twixt_strings=(float)(fretboard_top-fretboard_bottom)*fretboard_width_percent*2;

    frets.clear();
    GuitarUtils::getFretDistances(prefs.fretCount,scale,&fretDistances);
    frets.append(QLineF(
                       QPointF(scale,fretboard_top),
                       QPointF(scale,fretboard_bottom)
    ));
    for(float dist: fretDistances)
    {
        frets.append(QLineF(
                           QPointF(scale-dist,fretboard_top),
                           QPointF(scale-dist,fretboard_bottom)
        ));
    }

    int str=0;
    strings.clear();
    noteSource.clear();
    for(float y=fretboard_bottom+width_twixt_strings; y<fretboard_top-width_twixt_strings; y+=width_twixt_strings)
    {
        strings.append(QLineF(
                           QPointF(0,y),
                           QPointF(size().width(),y)
        ));

        quint8 start=prefs.strings[str];
        for(int i=0; i<frets.size()-1; i++)
        {
            QRectF noteBox(frets[i+1].p2(),frets[i].p1());
            float offset=noteBox.height()*.45f;
            noteBox.moveBottom(y+offset);
            noteSource[start+i]=noteBox;
        }
        str++;
    }

    update();
}

void GuitarInstrumentView::paintGL()
{
    painter.begin(this);

    QRectF dimensions(0,
                      height()*.325f,
                      width()*.325f,
                      height()/3
                      );
    guitarRenderer.render(&painter,dimensions);

    painter.setPen(fretColor);
    for(QLineF line: frets)
        painter.drawLine(line);

    painter.setPen(stringColor);
    for(QLineF line: strings)
        painter.drawLine(line);

    drawNotes(&noteSource);

    painter.end();
}
