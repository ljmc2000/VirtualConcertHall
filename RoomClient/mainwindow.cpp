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
}

MainWindow::~MainWindow()
{
    delete settingsWindow;
    delete ui;
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
