#ifndef INSTRUMENTVIEW_H
#define INSTRUMENTVIEW_H

#include <QIcon>
#include <QPainter>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QSvgRenderer>
#include <QTimer>

#include "roomcommon.h"

#define TIMEOUT 4000


using namespace RoomCommon;

namespace Ui {
class InstrumentView;
}

struct PianoKey
{
    QRectF dim;
    QColor color;
};

struct Note
{
    quint16 age=0;
    quint8 note;
};

class InstrumentView : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit InstrumentView(QWidget *parent = nullptr);
    ~InstrumentView();

    void fromPiano(quint8 minNote, quint8 maxNote),fromPiano();

public slots:
    void playNote(quint8 note);

protected:
    void paintGL();
    void initializeGL();
    void resizeGL(int w, int h);

private:
    Ui::InstrumentView *ui;
    QPainter painter;
    QSvgRenderer noteRenderer;
    QHash<quint8,PianoKey> keys;
    QTimer screenUpdateTimer;

    QList<Note> notes;
    quint8 minNote,maxNote;
};

#endif // INSTRUMENTVIEW_H
