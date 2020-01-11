#include "mainmenu.h"
#include "mainwindow.h"
#include "ui_mainmenu.h"
#include <QPushButton>

MainMenu::MainMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainMenu)
{
    ui->setupUi(this);

    connect(ui->joinButton, &QPushButton::clicked,
            [=](){ emit switchScreen(ROOMBROWSER) ;});

    connect(ui->createButton, &QPushButton::clicked,
           [=](){ emit switchScreen(ROOMCREATOR) ;});

    connect(ui->settingsButton, &QPushButton::clicked,
            [=](){ emit switchScreen(SETTINGS) ;});
}

MainMenu::~MainMenu()
{
    delete ui;
}
