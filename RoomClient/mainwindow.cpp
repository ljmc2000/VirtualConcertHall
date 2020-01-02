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
    this->hide();
}

void MainWindow::openPlayScreen()
{
    playScreen = new PlayScreen(this);
    playScreen->show();
    connect(playScreen, SIGNAL(destroyed()),
            this, SLOT(closePlayScreen()));
    this->hide();
}

void MainWindow::openLoginWindow()
{
    loginWindow = new LoginWindow(this,&httpApiClient);
    loginWindow->show();
    connect(loginWindow, SIGNAL(destroyed()),
            this, SLOT(closeLoginWindow()));
}

void MainWindow::closeSettings()
{
    settingsWindow=nullptr;
    show();
}

void MainWindow::closePlayScreen()
{
    playScreen=nullptr;
    show();
}

void MainWindow::closeLoginWindow()
{
    loginWindow=nullptr;
}
