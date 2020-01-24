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
    explicit InstrumentView(InstrumentType type, quint64 args, QWidget *parent = nullptr);
    ~InstrumentView();

    void updateInstrument();
    void setInstrument(InstrumentType type), setArgs(quint64 args);

public slots:
    void playNote(quint8 note);

protected:
    void paintGL();
    void initializeGL();
    void resizeGL(int w, int h);

private: //methods
    void fromPiano();
    void fromGuitar();

private:
    Ui::InstrumentView *ui;
    QPainter painter;
    QSvgRenderer noteRenderer,guitarBodyRenderer;
    QHash<quint8,PianoKey> keys;
    QTimer screenUpdateTimer;
    QList<Note> notes;


    InstrumentType instrumentType;
    quint64 instrumentArgs;
};

#endif // INSTRUMENTVIEW_H
