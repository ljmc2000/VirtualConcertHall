#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>

class Server: public QObject
{
    Q_OBJECT;

public:
    Server(int port);
    ~Server();

public slots:
    void readPendingDatagrams();

private:
    QUdpSocket qSocket;
};

#endif // SERVER_H
