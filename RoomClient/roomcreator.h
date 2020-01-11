#ifndef ROOMCREATOR_H
#define ROOMCREATOR_H

#include <QWidget>

#include "basescreen.h"
#include "httpapiclient.h"

namespace Ui {
class RoomCreator;
}

class RoomCreator : public QWidget
{
    Q_OBJECT

public:
    explicit RoomCreator(HttpAPIClient *httpApiClient, QWidget *parent = nullptr);
    ~RoomCreator();

signals:
    void switchScreen(Mode m);

private:
    Ui::RoomCreator *ui;
    HttpAPIClient *httpApiClient;
};

#endif // ROOMCREATOR_H
