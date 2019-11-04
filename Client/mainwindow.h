#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>

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
};

#endif // MAINWINDOW_H
