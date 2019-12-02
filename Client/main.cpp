#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("TUblin");
    QCoreApplication::setApplicationName("VirtualConcertHall");

    MainWindow w;
    w.show();

    return a.exec();
}
