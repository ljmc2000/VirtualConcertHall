#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <midihandler.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setPorts(std::vector<std::string>);

private:
    Ui::MainWindow *ui;
    QComboBox *comboBox;
    MidiHandler *m;
    QMetaObject::Connection m_connection;
};

#endif // MAINWINDOW_H
