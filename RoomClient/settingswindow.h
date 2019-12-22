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

    void setLastWindow(QMainWindow *w);

public slots:
    void setAddress();
    void setMidiInPort();
    void setMidiOutPort();
    void returnToLastWindow();

private:
    Ui::SettingsWindow *ui;

    QMainWindow *lastWindow;
    QSettings prefs;

    RtMidiIn midiin;
    RtMidiOut midiout;

private: //methods
    void setMidiPortsList();
};

#endif // SETTINGSWINDOW_H
