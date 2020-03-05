#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QSettings>

#include <RtMidi.h>
#include <fluidsynth.h>

#include "basescreen.h"
#include "guitarutils.h"
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
    static void midiSetArg(double timeStamp, std::vector<unsigned char> *message, void *userData);

private:
    Ui::SettingsWindow *ui;

    QSettings prefs;

    RtMidiIn midiin;

    InstrumentType instrumentType=PIANO;
    instrument_args_t instrumentArgs=0; quint8* changeAttr=0;
    QString audioDriver,soundfont;

    GuitarUtils guitarUtils;
    QMetaEnum tunings=QMetaEnum::fromType<GuitarTuning>();
    QMetaEnum DrumPadLayouts=QMetaEnum::fromType<DrumLayout>();

    HttpAPIClient *httpApiClient;
};

#endif // SETTINGSWINDOW_H
