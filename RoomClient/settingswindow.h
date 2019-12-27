#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QMainWindow>
#include <QSettings>

#include <RtMidi.h>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

    static void midiHandler( double timeStamp, std::vector<unsigned char> *message, void *userData );

public slots:
    void setAddress();
    void setMidiInPort();
    void setMidiOutPort();
    void returnToLastWindow();

private:
    Ui::SettingsWindow *ui;

    QSettings prefs;

    RtMidiIn midiin;
    RtMidiOut midiout;

    int maxNote=0, minNote=127;

private: //methods
    void setMidiPortsList();
};

#endif // SETTINGSWINDOW_H
