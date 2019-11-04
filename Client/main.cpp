#include "mainwindow.h"
#include <QApplication>
#include <midihandler.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MidiHandler *m = new MidiHandler(1);

    MainWindow w;
    w.setPorts(m->getPorts());
    w.show();

    return a.exec();
}
