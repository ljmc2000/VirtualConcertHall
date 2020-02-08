#include <QCoreApplication>
#include <servermanager.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ServerManager s;

    return a.exec();
}

