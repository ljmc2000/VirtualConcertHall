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

    connect(ui->createButton, &QPushButton::clicked,
            [=](){emit switchScreen(ROOMCREATOR);});
}

RoomBrowser::~RoomBrowser()
{
    delete ui;
}

void RoomBrowser::refreshRooms()
{
    QList<RoomInfo> rooms = httpApiClient->listRooms();
    ui->roomList->setRowCount(rooms.size());
    ui->roomList->setColumnCount(ROOMINFOATTRCOUNT);

    for(unsigned int i=0; i<rooms.count(); i++)
    {
        RoomInfo r=rooms[i];
        ui->roomList->itemAt(i,0)->setText(r.roomId);
        ui->roomList->itemAt(i,1)->setText(r.roomName);
        ui->roomList->itemAt(i,2)->setText(r.description);
        ui->roomList->itemAt(i,3)->setText(r.owner);
    }
}
