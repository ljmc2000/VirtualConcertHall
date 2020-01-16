#include "onlinestatus.h"
#include "ui_onlinestatus.h"

#define REFRESHINTERVAL (30*1000)

OnlineStatus::OnlineStatus(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::OnlineStatus)
{
    ui->setupUi(this);
}

OnlineStatus::~OnlineStatus()
{
    delete ui;
}

void OnlineStatus::setHttpApiClient(HttpAPIClient *httpApiClient)
{
    this->httpApiClient=httpApiClient;

    connect(httpApiClient, SIGNAL(httpError(int,QString)),
            this, SLOT(handleHttpError(int,QString)));

    connect(httpApiClient, SIGNAL(playerStateChange()),
            this, SLOT(update()));

    connect(ui->pushButton, SIGNAL(clicked()),
            this, SLOT(update()));

    refreshTimer.setInterval(REFRESHINTERVAL);
    connect(&refreshTimer, SIGNAL(timeout()),
            this, SLOT(update()));
    update();
    refreshTimer.start();
}

using namespace OnlineStatusNamespace;
void OnlineStatus::update()
{
    int state = httpApiClient->getUserStatus();

    if(state != -1)
    {
        emit changeState(this->state,(State)state);
        this->state=(State)state;
        ui->pushButton->setText(messages[state]);
        ui->pushButton->setIcon(icons[state]);
    }
}

State OnlineStatus::getState()
{
    return state;
}

void OnlineStatus::handleHttpError(int code,QString message)
{
    qDebug() << code << message;

    ui->pushButton->setText(messages[OFFLINE]);
    ui->pushButton->setIcon(icons[OFFLINE]);
}
