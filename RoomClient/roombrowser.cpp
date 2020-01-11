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


    for(int i=0; i<10; i++) for(int j=0; j<4; j++)
    {
        ui->roomList->setItem(i,j,&servers[i][j]);
    }
}

RoomBrowser::~RoomBrowser()
{
    delete ui;
}

void RoomBrowser::refreshRooms()
{
    QList<RoomInfo> rooms = httpApiClient->listRooms();

    for(int i=0; i<rooms.count(); i++)
    {
        RoomInfo r=rooms[i];
        servers[i][0].setText(r.roomId);
        servers[i][1].setText(r.roomName);
        servers[i][2].setText(r.description);
        servers[i][3].setText(r.owner);
    }

    ui->roomList->resizeColumnsToContents();
}
