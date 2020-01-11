#include "roomcreator.h"
#include "ui_roomcreator.h"

#include <QPushButton>

RoomCreator::RoomCreator(HttpAPIClient *httpApiClient,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoomCreator)
{
    ui->setupUi(this);
    this->httpApiClient=httpApiClient;

    connect(ui->backButton, &QPushButton::clicked,
            [=](){emit switchScreen(ROOMBROWSER);});
}

RoomCreator::~RoomCreator()
{
    delete ui;
}
