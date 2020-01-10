#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QSettings>

#include <RtMidi.h>
#include "httpapiclient.h"
#include "basescreen.h"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWindow(HttpAPIClient *httpApiClient,QWidget *parent = nullptr);
    ~SettingsWindow();

    static void midiHandler( double timeStamp, std::vector<unsigned char> *message, void *userData );

public slots:
    void setMidiInPort();
    void setMidiOutPort();
    void logout();

signals:
    void switchScreen(Mode mode);

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
