#ifndef INSTRUMENTVIEW_H
#define INSTRUMENTVIEW_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QSvgRenderer>
#include <QPainter>
#include <QIcon>
#include "roomcommon.h"

#define VIEWWIDTH 250
#define VIEWHEIGHT 300

using namespace RoomCommon;

namespace Ui {
class InstrumentView;
}

struct Position
{
    double x,y;
};

class InstrumentView : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit InstrumentView(QWidget *parent = nullptr);
    ~InstrumentView();

    void fromPiano(quint8 minNote, quint8 maxNote);

public slots:
    void playNote(quint8 note);

protected:
    void paintEvent(QPaintEvent *e);

private:
    Ui::InstrumentView *ui;
    static QString note;
    QIcon quaver;
    QPainter painter;
    QSvgRenderer renderer;

    QHash<quint8,Position> noteSource;
};

#endif // INSTRUMENTVIEW_H
