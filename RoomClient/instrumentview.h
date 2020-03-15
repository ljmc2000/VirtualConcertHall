#ifndef INSTRUMENTVIEW_H
#define INSTRUMENTVIEW_H

#include <QIcon>
#include <QPainter>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QSvgRenderer>
#include <QTimer>

#include "roomcommon.h"

#define NOTE_TIMEOUT 4000


using namespace RoomCommon;

namespace Ui {
class InstrumentView;
}

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
    static InstrumentView* getInstrumentView(InstrumentType type, instrument_args_t args, QWidget *parent);

    virtual void updateInstrument()=0;

public slots:
    void playNote(quint8 note);

protected:
    void initializeGL();
    void resizeGL(int w, int h);

    QPainter painter;
    QSvgRenderer noteRenderer;
    QList<Note> notes;

private:
    Ui::InstrumentView *ui;
    QTimer screenUpdateTimer;
};

#endif // INSTRUMENTVIEW_H
