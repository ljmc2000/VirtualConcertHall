#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <basescreen.h>
#include <httpapiclient.h>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(HttpAPIClient *httpApiClient, QWidget *parent = nullptr);
    ~LoginWindow();

public slots:
    void login();
    void handleError(QString error);

signals:
    void switchScreen(Mode mode);

private:
    Ui::LoginWindow *ui;
    HttpAPIClient *httpApiClient;
};

#endif // LOGIN_H
