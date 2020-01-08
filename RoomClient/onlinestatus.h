#ifndef ONLINESTATUS_H
#define ONLINESTATUS_H

#include "httpapiclient.h"
#include <QFrame>
#include <QIcon>
#include <QTimer>
#include <QMetaEnum>

namespace OnlineStatuses
{
    Q_NAMESPACE

    enum State
    {
        ONLINE,
        OFFLINE,
        INROOM,
    };
    Q_ENUM_NS(State);

    static QString messages[] =
    {
        "Online",
        "Offline",
        "In Room",
    };

    static QIcon icons[] =
    {
        QIcon(),
        QIcon(),
        QIcon(),
    };
};

namespace Ui {
class OnlineStatus;
}

class OnlineStatus : public QFrame
{
    Q_OBJECT

public:
    explicit OnlineStatus(QWidget *parent = nullptr);
    ~OnlineStatus();

public slots:
    void update();
    void handleHttpError(int code,QString message);

private:
    Ui::OnlineStatus *ui;

    QTimer refreshTimer;
    HttpAPIClient httpApiClient;
    QSize iconSize;
};

#endif // ONLINESTATUS_H
