#ifndef ROOMBROWSER_H
#define ROOMBROWSER_H

#include <QTableWidgetItem>
#include <QWidget>

#include "basescreen.h"
#include "httpapiclient.h"

#define PERPAGE 10

namespace Ui {
class RoomBrowser;
}

class RoomBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit RoomBrowser(HttpAPIClient *httpApiClient, QWidget *parent = nullptr);
    ~RoomBrowser();

public slots:
    void refreshRooms();
    void connectToRoom();
    void showError(QString error);

signals:
    void switchScreen(Mode mode);

private:
    Ui::RoomBrowser *ui;
    HttpAPIClient *httpApiClient;
    QTableWidgetItem servers[PERPAGE][ROOMINFOATTRCOUNT];
    RoomList rooms;

    int page=0;
};

#endif // ROOMBROWSER_H
