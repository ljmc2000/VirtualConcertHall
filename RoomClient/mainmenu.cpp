#include "mainmenu.h"
#include "ui_mainmenu.h"

#include <QPushButton>

#include "mainwindow.h"

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
