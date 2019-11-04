#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    comboBox = this->findChild<QComboBox*>("comboBox");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setPorts(std::vector<std::string> ports)
{
    for(unsigned int i=0; i<ports.size(); i++)
    {
        comboBox->addItem(ports.at(i).c_str());
    }
}
