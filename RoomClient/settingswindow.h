#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QMainWindow>
#include <QSettings>

#include <RtMidi.h>
#include "httpapiclient.h"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingsWindow(HttpAPIClient *httpApiClient,QWidget *parent = nullptr);
    ~SettingsWindow();

    static void midiHandler( double timeStamp, std::vector<unsigned char> *message, void *userData );

public slots:
    void setMidiInPort();
    void setMidiOutPort();
    void returnToLastWindow();
    void logout();
    void login();

private:
    Ui::SettingsWindow *ui;

    QSettings prefs;

    RtMidiIn midiin;
    RtMidiOut midiout;

    int maxNote=0, minNote=127;

    HttpAPIClient *httpApiClient;

private: //methods
    void setMidiPortsList();
    void refreshUsername();
};

#endif // SETTINGSWINDOW_H
