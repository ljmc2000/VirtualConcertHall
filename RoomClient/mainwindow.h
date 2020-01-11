#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "basescreen.h"
#include "httpapiclient.h"
#include "onlinestatus.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void showEvent(QShowEvent *ev);
    void resizeEvent(QResizeEvent *ev);

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
