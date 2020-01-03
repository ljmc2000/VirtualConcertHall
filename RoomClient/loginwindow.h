#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <httpapiclient.h>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr,QString error="Please sign in");
    ~LoginWindow();

public slots:
    void login();
    void handleError(QString error);

private:
    Ui::LoginWindow *ui;
    HttpAPIClient httpApiClient;
};

#endif // LOGIN_H
