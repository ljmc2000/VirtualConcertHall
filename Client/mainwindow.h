#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <settingswindow.h>

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

private:
    Ui::MainWindow *ui;

    SettingsWindow settingsWindow;
};

#endif // MAINWINDOW_H
