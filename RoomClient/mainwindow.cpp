#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "mainmenu.h"
#include "playscreen.h"
#include "settingswindow.h"
#include "loginwindow.h"
#include "roombrowser.h"
#include "roomcreator.h"
#include "onlinestatusnamespace.h"

using namespace OnlineStatusNamespace;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->onlineStatus->setHttpApiClient(&httpApiClient);

    switch (ui->onlineStatus->getState())
    {
    case INROOM:
        openWidget(PLAYSCREEN);
        break;
    case NOLOGIN:
        openWidget(LOGIN);
        break;
    default:
        currentMode=LOGIN;
        openWidget(MAINMENU);
        break;
    }

    connect(&httpApiClient,SIGNAL(apiError(QString)),
            this,SLOT(handleError(QString)));

    connect(&httpApiClient,SIGNAL(httpError(int,QString)),
            this,SLOT(handleError(int,QString)));

    connect(&httpApiClient,SIGNAL(tokenChange()),
            ui->onlineStatus,SLOT(update()));

    connect(ui->onlineStatus, SIGNAL(changeState(State)),
            this, SLOT(handleChangeOnlineState(State)));
}

MainWindow::~MainWindow()
{
    delete activeWidget;
    delete ui;
}

void MainWindow::openWidget(Mode mode)
{
    if(currentMode!=mode)
    {
        currentMode=mode;
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
        case ROOMBROWSER:
            activeWidget=new RoomBrowser(&httpApiClient,this);
            break;
        case ROOMCREATOR:
            activeWidget=new RoomCreator(&httpApiClient,this);
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

        activeWidget->setParent(ui->frame);
        ui->frame->setMinimumSize(activeWidget->size());
        activeWidget->resize(ui->frame->size());
        activeWidget->show();
        ui->onlineStatus->update();
    }
}

void MainWindow::showEvent(QShowEvent *ev)
{
    QMainWindow::showEvent(ev);
    activeWidget->resize(ui->frame->size());
}

void MainWindow::resizeEvent(QResizeEvent *ev)
{
    QMainWindow::resizeEvent(ev);
    activeWidget->resize(ui->frame->size());
}

void MainWindow::handleChangeOnlineState(State state)
{
    switch (state)
    {
    case INROOM:
        openWidget(PLAYSCREEN);
        break;
    case NOLOGIN:
        openWidget(LOGIN);
        break;
    default:
        emit changeOnlineState(state);
        break;
    }
}

void MainWindow::handleError(QString error)
{
    qDebug() << error;
}

void MainWindow::handleError(int code,QString error)
{
    qDebug() << code << error;
}
