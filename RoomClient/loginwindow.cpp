#include "loginwindow.h"
#include "ui_loginwindow.h"

LoginWindow::LoginWindow(HttpAPIClient *httpApiClient,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    this->httpApiClient=httpApiClient;

    connect(ui->loginButton,SIGNAL(clicked()),
            this, SLOT(login()));

    connect(httpApiClient,SIGNAL(apiError(QString)),
            this, SLOT(handleError(QString)));
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::login()
{
    QString username = ui->usernameBox->text();
    QString password = ui->passwordBox->text();

    if(httpApiClient->signin(username,password)) emit switchScreen(SETTINGS);
}

void LoginWindow::handleError(QString error)
{
    //qDebug() << error;
    if(error.size() != 0)
        ui->errorLabel->setText(error);
}
