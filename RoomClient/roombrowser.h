#ifndef ROOMBROWSER_H
#define ROOMBROWSER_H

#include <QWidget>
#include <QTableWidgetItem>
#include "httpapiclient.h"
#include "basescreen.h"

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

signals:
    void switchScreen(Mode mode);

private:
    Ui::RoomBrowser *ui;
    HttpAPIClient *httpApiClient;
    QTableWidgetItem servers[10][4];
};

#endif // ROOMBROWSER_H
