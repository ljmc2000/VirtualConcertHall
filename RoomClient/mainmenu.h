#ifndef MAINMENU_H
#define MAINMENU_H

#include "basescreen.h"
#include <QWidget>

namespace Ui {
class MainMenu;
}

class MainMenu : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenu(QWidget *parent = nullptr);
    ~MainMenu();

signals:
    void switchScreen(Mode mode);

private:
    Ui::MainMenu *ui;
};

#endif // MAINMENU_H
