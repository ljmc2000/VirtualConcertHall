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
    delete ui;
}

void MainWindow::openSettings()
{
    settingsWindow.show();
    settingsWindow.setLastWindow(this);
    this->hide();
}

void MainWindow::openPlayScreen()
{
    playScreen.show();
    playScreen.setLastWindow(this);
    this->hide();
}
