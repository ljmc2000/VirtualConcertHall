#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QSettings>

#include <RtMidi.h>
#include "httpapiclient.h"
#include "instrumentview.h"
#include "basescreen.h"
#include "onlinestatusnamespace.h"

using namespace OnlineStatusNamespace;

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
    void setSoundFont();
    void setInstrumentType();
    void logout();
    void refreshUsername();

signals:
    void switchScreen(Mode mode);

private:
    Ui::SettingsWindow *ui;

    QSettings prefs;

    RtMidiIn midiin;
    RtMidiOut midiout;

    int maxNote=0, minNote=127;
    InstrumentType instrumentType;

    HttpAPIClient *httpApiClient;

private: //methods
    void setMidiPortsList();
    void renderInstrument();
};

#endif // SETTINGSWINDOW_H
