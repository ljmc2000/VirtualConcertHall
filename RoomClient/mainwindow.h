#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "httpapiclient.h"
#include "onlinestatus.h"
#include "basescreen.h"

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
    void openWidget(Mode mode);

private slots:
    void handleError(QString);
    void handleError(int,QString);

private:
    Ui::MainWindow *ui;

    QWidget *activeWidget=nullptr;
    HttpAPIClient httpApiClient;
};

#endif // MAINWINDOW_H
