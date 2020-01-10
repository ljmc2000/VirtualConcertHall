#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "mainmenu.h"
#include "playscreen.h"
#include "settingswindow.h"
#include "loginwindow.h"
#include "roombrowser.h"
#include "onlinestatusnamespace.h"

using namespace OnlineStatusNamespace;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->onlineStatus->setHttpApiClient(&httpApiClient);



    connect(&httpApiClient,SIGNAL(apiError(QString)),
            this,SLOT(handleError(QString)));

    connect(&httpApiClient,SIGNAL(httpError(int,QString)),
            this,SLOT(handleError(int,QString)));

    openWidget(httpApiClient.test() ? MAINMENU:LOGIN);
}

MainWindow::~MainWindow()
{
    delete activeWidget;
    delete ui;
}

void MainWindow::openWidget(Mode mode)
{
    delete activeWidget;

    switch(mode)
    {
    case LOGIN:
        activeWidget=new LoginWindow(&httpApiClient,this);
        break;
    case MAINMENU:
        activeWidget=new MainMenu(this);
        break;
    case SETTINGS:
        activeWidget=new SettingsWindow(&httpApiClient,this);
        break;
    case PLAYSCREEN:
        if (ui->onlineStatus->getState()==INROOM)
        {
            RoomConnectionInfo r=httpApiClient.getCurrentRoom();
            activeWidget=new PlayScreen(r.secretId,r.roomIp,r.roomPort,this);
        }
        else
        {
            activeWidget=new RoomBrowser(&httpApiClient,this);
        }

        break;
    }

    connect(activeWidget, SIGNAL(switchScreen(Mode)),
            this, SLOT(openWidget(Mode)));

    activeWidget->show();
    ui->onlineStatus->update();
}

void MainWindow::handleError(QString error)
{
    qDebug() << error;
}

void MainWindow::handleError(int code,QString error)
{
    qDebug() << code << error;
}
