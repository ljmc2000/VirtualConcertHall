#ifndef ONLINESTATUS_H
#define ONLINESTATUS_H

#include "httpapiclient.h"
#include "onlinestatusnamespace.h"
#include <QFrame>
#include <QIcon>
#include <QTimer>

using namespace OnlineStatusNamespace;

namespace Ui {
class OnlineStatus;
}

class OnlineStatus : public QFrame
{
    Q_OBJECT

public:
    explicit OnlineStatus(QWidget *parent = nullptr);
    ~OnlineStatus();

    void setHttpApiClient(HttpAPIClient *httpApiClient);
    OnlineStatusNamespace::State getState();

public slots:
    void update();
    void handleHttpError(int code,QString message);

signals:
    void changeState(State old,State current);

private:
    Ui::OnlineStatus *ui;

    QTimer refreshTimer;
    HttpAPIClient *httpApiClient;
    QSize iconSize;
    State state=OFFLINE;
};

#endif // ONLINESTATUS_H
