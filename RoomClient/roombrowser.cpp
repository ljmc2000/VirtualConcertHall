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

    connect(ui->connectButton, SIGNAL(clicked()),
            this, SLOT(connectToRoom()));

    connect(ui->backButton, &QPushButton::clicked,
            [=](){page--; refreshRooms();});
    connect(ui->nextButton, &QPushButton::clicked,
            [=](){page++; refreshRooms();});

    ui->roomList->setRowCount(PERPAGE);
    for(int i=0; i<PERPAGE; i++) for(int j=0; j<ROOMINFOATTRCOUNT; j++)
    {
        ui->roomList->setItem(i,j,&servers[i][j]);
    }

    refreshRooms();
}

RoomBrowser::~RoomBrowser()
{
    ui->roomList->deleteLater();
    delete ui;
}

void RoomBrowser::refreshRooms()
{
    RoomList rooms = httpApiClient->listRooms(page,PERPAGE);

    int i;
    for(i=0; i<rooms.results.count(); i++)
    {
        RoomInfo r= i<PERPAGE? rooms.results[i]:RoomInfo();
        servers[i][0].setText(r.roomId);
        servers[i][1].setText(r.roomName);
        servers[i][2].setText(r.description);
        servers[i][3].setText(r.owner);
    }

    ui->roomList->resizeColumnsToContents();
    ui->lastButton->setEnabled(page!=0);
    ui->nextButton->setEnabled(rooms.more);
}

void RoomBrowser::connectToRoom()
{
    int room=ui->roomList->currentRow();
    QString roomId=servers[room][0].text();

    if(roomId.size()!=0)
    {
        httpApiClient->joinRoom(roomId);
    }
}
