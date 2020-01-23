#ifndef BASESCREEN_H
#define BASESCREEN_H

#define BASESCREEN signals: \
    void switchScreen(Mode mode);

enum Mode{MAINMENU,LOGIN,PLAYSCREEN,SETTINGS,ROOMBROWSER,ROOMCREATOR};

#endif // BASESCREEN_H
