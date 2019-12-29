#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QTimer>
#include <QDateTime>

struct Client
{
    Client(QHostAddress address, int port)
    {
        this->address=address;
        this->port=port;
    }

    bool operator==(const Client& c){return address==c.address && port==c.port;}

    QHostAddress address;
    quint16 port;
};

class Server: public QObject
{
    Q_OBJECT;

public:
    Server(int port);
    ~Server();

public slots:
    void readPendingDatagrams();
    void heartBeat();
    void pruneClients();

private:
    QUdpSocket qSocket;
    QTimer heartBeatTimer;
    QTimer pruneTimer;
    QList<Client> clients;
    QList<qint64> lastMessage;

    void sendToAll(QByteArray data);
    void addClient(Client c);
    void updateNumbers();
    void disconnectClient(int index);
};

#endif // SERVER_H
