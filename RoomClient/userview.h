#ifndef USERVIEW_H
#define USERVIEW_H

#include <QFrame>
#include "httpapiclient.h"
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
    InstrumentType getInstrumentType();

    float volume=1;

public slots:
    void setUsername(QString username);

private:
    Ui::UserView *ui;
    InstrumentView *instrumentView;
};

#endif // USERVIEW_H
