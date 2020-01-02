#include "loginwindow.h"
#include "ui_loginwindow.h"

LoginWindow::LoginWindow(QWidget *parent, HttpAPIClient *httpApiClient) :
    QDialog(parent),
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

    if(httpApiClient->signin(username,password)) delete this;
}

void LoginWindow::handleError(QString error)
{
    //qDebug() << error;
    ui->errorLabel->setText(error);
}
