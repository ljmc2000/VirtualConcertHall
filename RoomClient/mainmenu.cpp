#include "mainmenu.h"
#include "mainwindow.h"
#include "ui_mainmenu.h"
#include <QPushButton>

MainMenu::MainMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainMenu)
{
    ui->setupUi(this);

    connect(ui->playButton, &QPushButton::clicked,
            [=](){ emit switchScreen(PLAYSCREEN) ;});

    connect(ui->settingsButton, &QPushButton::clicked,
            [=](){ emit switchScreen(SETTINGS) ;});
}

MainMenu::~MainMenu()
{
    delete ui;
}
