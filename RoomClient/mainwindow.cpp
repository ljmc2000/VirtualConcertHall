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
        openWidget(MAINMENU);
        break;
    }

    connect(&httpApiClient,SIGNAL(apiError(QString)),
            this,SLOT(handleError(QString)));

    connect(&httpApiClient,SIGNAL(httpError(int,QString)),
            this,SLOT(handleError(int,QString)));

    connect(ui->onlineStatus, SIGNAL(changeState(State,State)),
            this, SLOT(handleChangeOnlineState(State,State)));
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
    case ROOMBROWSER:
        activeWidget=new RoomBrowser(&httpApiClient,this);
        break;
    case ROOMCREATOR:
        activeWidget=new RoomCreator(&httpApiClient,this);
        break;
    case PLAYSCREEN:
        RoomConnectionInfo r = httpApiClient.getCurrentRoom();
        activeWidget=new PlayScreen(r,&httpApiClient,this);
        break;
    }

    connect(activeWidget, SIGNAL(switchScreen(Mode)),
            this, SLOT(openWidget(Mode)));

    activeWidget->setParent(ui->frame);
    ui->frame->setMinimumSize(activeWidget->size());
    activeWidget->resize(ui->frame->size());
    activeWidget->show();
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

void MainWindow::handleChangeOnlineState(State old, State current)
{
    if(old!=current)
    {
        switch(current)
        {
        case INROOM:
            openWidget(PLAYSCREEN);
            break;
        case NOLOGIN:
            openWidget(LOGIN);
            break;
        default:
            break;
        }
    }

    //emit changeOnlineState(current);
}

void MainWindow::handleError(QString error)
{
    qDebug() << error;
}

void MainWindow::handleError(int code,QString error)
{
    qDebug() << code << error;
}
