#include <QCoreApplication>
#include "client.h"

int main(int argc, char *argv[])
{
    if(argc==8)
    {
        QCoreApplication a(argc, argv);
        RoomConnectionInfo r;
        r.roomIp=argv[1];
        r.roomPort=QString(argv[2]).toUShort();
        r.roomId=QString(argv[3]).toULong();
        r.secretId=QString(argv[4]).toLong();
        QString username(argv[5]);
        InstrumentType instrumentType=(InstrumentType) QString(argv[6]).toUShort();
        instrument_args_t instrumentArgs=QString(argv[7]).toInt(nullptr,16);
        Client c(r,username,instrumentType,instrumentArgs);

        return a.exec();
    }

    else
    {
        qDebug() << "usage: ./binary serverip port roomId secretId username instrumentType instrumentArgs";
    }
}
