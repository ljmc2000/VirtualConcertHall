#include "roombrowser.h"
#include "ui_roombrowser.h"

#include <QPushButton>

RoomBrowser::RoomBrowser(HttpAPIClient *httpApiClient, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoomBrowser)
{
    ui->setupUi(this);
    this->httpApiClient = httpApiClient;

    connect(ui->backButton, &QPushButton::clicked,
            [=](){emit switchScreen(MAINMENU);});

    connect(ui->refreshButton, SIGNAL(clicked()),
            this, SLOT(refreshRooms()));
}

RoomBrowser::~RoomBrowser()
{
    delete ui;
}

void RoomBrowser::refreshRooms()
{

}
