#-------------------------------------------------
#
# Project created by QtCreator 2019-11-04T18:04:33
#
#-------------------------------------------------

QT       += core gui network svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    main.cpp \
    midihandler.cpp \
    $$PWD/../RoomCommon/httpapiclient.cpp \
    settingswindow.cpp \
    mainwindow.cpp \
    playscreen.cpp \
    loginwindow.cpp \
    onlinestatus.cpp \
    mainmenu.cpp \
    roombrowser.cpp \
    roomcreator.cpp \
    instrumentview.cpp

HEADERS += \
    midihandler.h \
    $$PWD/../RoomCommon/roomcommon.h \
    $$PWD/../RoomCommon/httpapiclient.h \
    $$PWD/../RoomCommon/onlinestatusnamespace.h \
    settingswindow.h \
    mainwindow.h \
    playscreen.h \
    loginwindow.h \
    onlinestatus.h \
    mainmenu.h \
    basescreen.h \
    roombrowser.h \
    roomcreator.h \
    instrumentview.h

FORMS += \
    settingswindow.ui \
    mainwindow.ui \
    playscreen.ui \
    loginwindow.ui \
    onlinestatus.ui \
    mainmenu.ui \
    roombrowser.ui \
    roomcreator.ui \
    instrumentview.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#common files shared with server
INCLUDEPATH +=$$PWD/../RoomCommon

#rtmidi
win32: LIBS += -L$$PWD/rtmidiWindows/ -lrtmidi
else:unix: LIBS += -L$$PWD/rtmidiLinux/ -lrtmidi

INCLUDEPATH += $$PWD/rtmidi
DEPENDPATH += $$PWD/rtmidi

#fluidsynth
win32:LIBS += -L$$PWD/fluidsynthWindows/src -lfluidsynth
else:unix: LIBS += -L$$PWD/fluidsynthLinux/src -lfluidsynth

win32:INCLUDEPATH += $$PWD/fluidsynthWindows/include/ $$PWD/fluidsynthWindows/include/fluidsynth
else:unix:INCLUDEPATH += $$PWD/fluidsynthLinux/include/ $$PWD/fluidsynthLinux/include/fluidsynth
INCLUDEPATH += $$PWD/fluidsynth/include/
