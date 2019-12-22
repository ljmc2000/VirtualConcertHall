#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QTimer>

struct Client
{
    Client(QHostAddress address, int port)
    {
        this->address=address;
        this->port=port;
    }

    auto operator==(const Client& c){return address==c.address && port==c.port;}

    QHostAddress address;
    int port;
};

class Server: public QObject
{
    Q_OBJECT;

public:
    Server(int port);
    ~Server();

public slots:
    void readPendingDatagrams();
    void heartBeatTimeout();

private:
    QUdpSocket qSocket;
    QTimer heartBeatTimeoutTimer;
    QList<Client> clients;
    QList<int> clientTimeouts;

    int lastClientId=0;

    void sendMidiToClients(QByteArray data);
};

#endif // SERVER_H
