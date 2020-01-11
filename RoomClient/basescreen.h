#ifndef BASESCREEN_H
#define BASESCREEN_H

enum Mode{MAINMENU,LOGIN,PLAYSCREEN,SETTINGS,ROOMBROWSER,ROOMCREATOR};

#define BASESCREEN signals: \
    void switchScreen(Mode mode);

#endif // BASESCREEN_H
