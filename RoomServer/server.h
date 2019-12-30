#ifndef SERVER_H
#define SERVER_H

#define GETTIME() QDateTime::currentDateTime().toMSecsSinceEpoch()

#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QTimer>
#include <QDateTime>

struct Client
{
    Client(QHostAddress address, quint16 port)
    {
        this->address=address;
        this->port=port;
    }

    friend bool operator==(Client a,const Client& c){return a.address==c.address && a.port==c.port;}
    friend uint qHash(const Client &c){return qHash(c.address,c.port);}

    QHostAddress address;
    quint16 port;
};

struct ClientData
{
    quint8 clientId;
    qint64 lastMessage=GETTIME();
    bool awake=true;
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
    QHash<Client,ClientData> clients;
    quint8 nextClientId=0;

    quint8 getNextClientId();
    void sendToAll(QByteArray data);
    void addClient(Client c);
    void disableClient(Client c);
    void enableClient(Client c);
};

#endif // SERVER_H
