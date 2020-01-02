#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <httpapiclient.h>
#include <playscreen.h>
#include <settingswindow.h>
#include <loginwindow.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void openSettings();
    void openPlayScreen();
    void openLoginWindow();

private slots:
    void closeSettings();
    void closePlayScreen();
    void closeLoginWindow();

private:
    Ui::MainWindow *ui;

    PlayScreen *playScreen=nullptr;
    SettingsWindow *settingsWindow=nullptr;
    LoginWindow *loginWindow=nullptr;
    HttpAPIClient httpApiClient;
};

#endif // MAINWINDOW_H
