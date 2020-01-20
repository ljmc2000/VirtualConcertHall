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
    void initializeGL();

private:
    Ui::InstrumentView *ui;
    QPainter painter;
    QSvgRenderer instrumentRenderer,noteRenderer;

    QHash<quint8,QPointF> noteSource;
    QList<QPointF> notes;
    QSizeF noteSize;
};

#endif // INSTRUMENTVIEW_H
