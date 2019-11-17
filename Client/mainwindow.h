#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

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

public slots:
    void setAddress();

private:
    Ui::MainWindow *ui;
    QComboBox *comboBox;
    QUdpSocket qSocket;
    MidiHandler *midiHandler;
    QLineEdit *serverIPBox;
    QPushButton *connectButton;
};

#endif // MAINWINDOW_H
