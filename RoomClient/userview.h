#ifndef USERVIEW_H
#define USERVIEW_H

#include <QWidget>
#include "instrumentview.h"

namespace Ui {
class UserView;
}

class UserView : public QWidget
{
    Q_OBJECT

public:
    explicit UserView(InstrumentView *v, QWidget *parent = nullptr);
    ~UserView();

    void playNote(quint8 midiMessage);

    float volume=1;

private:
    Ui::UserView *ui;
    InstrumentView *instrumentView;
};

#endif // USERVIEW_H
