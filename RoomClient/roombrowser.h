#ifndef ROOMBROWSER_H
#define ROOMBROWSER_H

#include <QWidget>
#include <QTableWidgetItem>
#include "httpapiclient.h"
#include "basescreen.h"

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

signals:
    void switchScreen(Mode mode);

private:
    Ui::RoomBrowser *ui;
    HttpAPIClient *httpApiClient;
    QTableWidgetItem servers[PERPAGE][ROOMINFOATTRCOUNT];

    int page=0;
    bool more=true;
};

#endif // ROOMBROWSER_H
