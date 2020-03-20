#ifndef USERVIEW_H
#define USERVIEW_H

#include <QFrame>
#include "instrumentview.h"

namespace Ui {
class UserView;
}

class UserView : public QFrame
{
    Q_OBJECT

public:
    explicit UserView(InstrumentView *v, QString username, QWidget *parent = nullptr);
    ~UserView();

    void playNote(quint8 midiMessage);
    void setLatency(qint16 latency);

    float volume=1;

private:
    Ui::UserView *ui;
    InstrumentView *instrumentView;
};

#endif // USERVIEW_H
