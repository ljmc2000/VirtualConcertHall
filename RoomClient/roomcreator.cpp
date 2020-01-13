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
            [=](){emit switchScreen(MAINMENU);});

    connect(ui->confirmButton, SIGNAL(clicked()),
            this, SLOT(createRoom()));
}

RoomCreator::~RoomCreator()
{
    delete ui;
}

void RoomCreator::createRoom()
{
    QString roomname = ui->roomNameBox->text();
    QString description = ui->descriptionBox->toPlainText();
    QString password=ui->passwordBox->text(), confirmPassword=ui->confirmPasswordBox->text();
    bool isprivate = ui->privateCheckBox->checkState();

    if(roomname.length()==0)
    {
        ui->errorLabel->setText("A room name is not optional");
    }
    else if(password!=confirmPassword)
    {
        ui->errorLabel->setText("Passwords must match");
    }
    else
    {
        httpApiClient->createRoom(roomname,
                                  description.length()!=0 ? description:nullptr,
                                  password.length()!=0 ? password:nullptr,
                                  isprivate);
    }
}
