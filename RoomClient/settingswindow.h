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
    void redrawInstrument();
    void logout();
    void refreshUsername();

signals:
    void switchScreen(Mode mode);
    void instrumentUpdate();

private: //methods
    void setDefaults();
    void setMidiPortsList();
    void showInstrumentConfig();
    void clearInstrumentConfig();
    static void setDriverList(void *data, const char *name, const char* value);
    static void midiHandler( double timeStamp, std::vector<unsigned char> *message, void *userData );

private: //piano specific methods
    static void pianoSetMinNote(double timeStamp, std::vector<unsigned char> *message, void *userData);
    static void pianoSetMaxNote(double timeStamp, std::vector<unsigned char> *message, void *userData);

private:
    Ui::SettingsWindow *ui;

    QSettings prefs;

    RtMidiIn midiin;

    InstrumentType instrumentType=PIANO;
    quint64 instrumentArgs=0;
    QString audioDriver,soundfont;

    HttpAPIClient *httpApiClient;
};

#endif // SETTINGSWINDOW_H
