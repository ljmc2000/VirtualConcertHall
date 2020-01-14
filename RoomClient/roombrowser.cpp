#include "roombrowser.h"
#include "ui_roombrowser.h"

#include <QPushButton>
#include <QInputDialog>

RoomBrowser::RoomBrowser(HttpAPIClient *httpApiClient, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoomBrowser)
{
    ui->setupUi(this);
    this->httpApiClient = httpApiClient;

    connect(httpApiClient, &HttpAPIClient::apiError,
            [=](QString error){ui->errorBox->setText(error);});

    connect(ui->backButton, &QPushButton::clicked,
            [=](){emit switchScreen(MAINMENU);});

    connect(ui->refreshButton, SIGNAL(clicked()),
            this, SLOT(refreshRooms()));

    connect(ui->connectButton, SIGNAL(clicked()),
            this, SLOT(connectToRoom()));

    connect(ui->lastButton, &QPushButton::clicked,
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
    rooms = httpApiClient->listRooms(page,PERPAGE);

    int i;
    for(i=0; i<rooms.results.count(); i++)
    {
        RoomInfo r= i<PERPAGE? rooms.results[i]:RoomInfo();
        servers[i][roomId].setText(r.roomId);
        servers[i][roomName].setText(r.roomName);
        servers[i][description].setText(r.description);
        servers[i][owner].setText(r.owner);
        servers[i][password].setText(r.password? "Required":"");
    }

    ui->roomList->resizeColumnsToContents();
    ui->lastButton->setEnabled(page!=0);
    ui->nextButton->setEnabled(rooms.more);
}

void RoomBrowser::connectToRoom()
{
    int index=ui->roomList->currentRow();
    if(index<rooms.results.count())
    {
        RoomInfo room=rooms.results[index];
        QString password=nullptr;

        if(room.password)
        {
            password=QInputDialog::getText(this,"Enter password for room","Password",QLineEdit::Password);
        }

        httpApiClient->joinRoom(room.roomId,password);
    }
}
