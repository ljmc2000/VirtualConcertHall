#include "roombrowser.h"
#include "ui_roombrowser.h"

#include <QPushButton>

RoomBrowser::RoomBrowser(HttpAPIClient *httpApiClient, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoomBrowser)
{
    ui->setupUi(this);
    this->httpApiClient = httpApiClient;

    connect(httpApiClient, &HttpAPIClient::more,
            [=](bool more){this->more=more;});

    connect(ui->backButton, &QPushButton::clicked,
            [=](){emit switchScreen(MAINMENU);});

    connect(ui->refreshButton, SIGNAL(clicked()),
            this, SLOT(refreshRooms()));

    connect(ui->connectButton, SIGNAL(clicked()),
            this, SLOT(connectToRoom()));

    connect(ui->backButton, &QPushButton::clicked,
            [=](){if(page>0){page--; refreshRooms();}});
    connect(ui->nextButton, &QPushButton::clicked,
            [=](){if(more){page++; refreshRooms();}});


    for(int i=0; i<10; i++) for(int j=0; j<4; j++)
    {
        ui->roomList->setItem(i,j,&servers[i][j]);
    }
    refreshRooms();
}

RoomBrowser::~RoomBrowser()
{
    delete ui;
}

void RoomBrowser::refreshRooms()
{
    QList<RoomInfo> rooms = httpApiClient->listRooms(page,PERPAGE);

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

void RoomBrowser::connectToRoom()
{
    int room=ui->roomList->currentRow();
    QString roomId=servers[room][0].text();

    if(roomId.size()!=0)
    {
        connect(httpApiClient, &HttpAPIClient::roomReady,
                [=](){emit switchScreen(PLAYSCREEN);});

        httpApiClient->joinRoom(roomId);
    }
}
