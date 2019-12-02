#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->settingsButton,SIGNAL(clicked()),
            this,SLOT(openSettings()));
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
