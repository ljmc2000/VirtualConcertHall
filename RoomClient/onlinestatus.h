#ifndef ONLINESTATUS_H
#define ONLINESTATUS_H

#include "httpapiclient.h"
#include "onlinestatusnamespace.h"
#include <QFrame>
#include <QIcon>
#include <QTimer>

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
