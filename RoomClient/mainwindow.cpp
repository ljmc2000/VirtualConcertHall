#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->settingsButton,SIGNAL(clicked()),
            this,SLOT(openSettings()));

    connect(ui->playButton, SIGNAL(clicked()),
            this, SLOT(openPlayScreen()));

    connect(&httpApiClient,SIGNAL(apiError(QString)),
            this,SLOT(handleError(QString)));

    connect(&httpApiClient,SIGNAL(httpError(int,QString)),
            this,SLOT(handleError(int,QString)));

    if(!httpApiClient.test()) openLoginWindow();
}

MainWindow::~MainWindow()
{
    delete settingsWindow;
    delete ui;
    delete loginWindow;
}

void MainWindow::openSettings()
{
    settingsWindow = new SettingsWindow(this);
    settingsWindow->show();
    connect(settingsWindow, SIGNAL(destroyed()),
            this, SLOT(closeSettings()));
}

void MainWindow::openPlayScreen()
{
    RoomConnectionInfo r=httpApiClient.getCurrentRoom();
    playScreen = new PlayScreen(r.secretId,r.roomIp,r.roomPort,this);
    playScreen->show();
    connect(playScreen, SIGNAL(destroyed()),
            this, SLOT(closePlayScreen()));
    this->hide();
}

void MainWindow::openLoginWindow()
{
    loginWindow = new LoginWindow(this);
    loginWindow->show();
    connect(loginWindow, SIGNAL(destroyed()),
            this, SLOT(closeLoginWindow()));
}

void MainWindow::closeSettings()
{
    settingsWindow=nullptr;
    ui->onlineStatus->update();
}

void MainWindow::closePlayScreen()
{
    playScreen=nullptr;
    show();
}

void MainWindow::closeLoginWindow()
{
    loginWindow=nullptr;
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
