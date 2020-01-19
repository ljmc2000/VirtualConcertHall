#ifndef INSTRUMENTVIEW_H
#define INSTRUMENTVIEW_H

#include <QSvgWidget>
#include "roomcommon.h"

#define VIEWWIDTH 250
#define VIEWHEIGHT 300

using namespace RoomCommon;

namespace Ui {
class InstrumentView;
}

class InstrumentView : public QSvgWidget
{
    Q_OBJECT

public:
    explicit InstrumentView(QWidget *parent = nullptr);
    ~InstrumentView();

    void fromPiano(quint8 minNote, quint8 maxNote);

private:
    Ui::InstrumentView *ui;
    static QString note;
};

#endif // INSTRUMENTVIEW_H
