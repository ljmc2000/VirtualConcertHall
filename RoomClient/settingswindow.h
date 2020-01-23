#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QSettings>

#include <RtMidi.h>
#include <fluidsynth.h>

#include "basescreen.h"
#include "httpapiclient.h"
#include "instrumentview.h"
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

public slots:
    void setMidiInPort();
    void setSoundFont();
    void setAudioDriver();
    void setInstrumentType();
    void logout();
    void refreshUsername();

signals:
    void switchScreen(Mode mode);

private:
    Ui::SettingsWindow *ui;

    QSettings prefs;

    RtMidiIn midiin;
    fluid_synth_t *midiout;
    fluid_audio_driver_t *soundout;

    int maxNote=0, minNote=127;
    InstrumentType instrumentType;

    HttpAPIClient *httpApiClient;

private: //methods
    void setDefaults();
    void setMidiPortsList();
    void renderInstrument();
    void initSynth(), closeSynth();
    static void setDriverList(void *data, const char *name, const char* value);
    static void midiHandler( double timeStamp, std::vector<unsigned char> *message, void *userData );
};

#endif // SETTINGSWINDOW_H
