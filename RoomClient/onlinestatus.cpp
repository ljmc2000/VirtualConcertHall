#include "onlinestatus.h"
#include "ui_onlinestatus.h"

#define REFRESHINTERVAL (15*60*1000)

OnlineStatus::OnlineStatus(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::OnlineStatus)
{
    ui->setupUi(this);

    this->iconSize=ui->statusIcon->size();

    connect(&httpApiClient, SIGNAL(httpError(int,QString)),
            this, SLOT(handleHttpError(int,QString)));

    refreshTimer.setInterval(REFRESHINTERVAL);
    connect(&refreshTimer, SIGNAL(timeout()),
            this, SLOT(update()));
    update();
    refreshTimer.start();
}

OnlineStatus::~OnlineStatus()
{
    delete ui;
}

using namespace OnlineStatusNamespace;
void OnlineStatus::update()
{
    int state = httpApiClient.getUserStatus();

    if(state != -1)
    {
        ui->statusLabel->setText(messages[state]);
        ui->statusIcon->setPixmap(icons[state].pixmap(iconSize));
    }
}

void OnlineStatus::handleHttpError(int code,QString message)
{
    qDebug() << code << message;

    ui->statusLabel->setText(messages[OFFLINE]);
    ui->statusIcon->setPixmap(icons[OFFLINE].pixmap(iconSize));
}
