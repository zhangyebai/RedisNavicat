#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    void initMainWindow();
    void initMainSignals();
private:
    Ui::MainWindow *ui;




private:
    bool m_wndMax = false;
    bool m_wndTop = false;

private:
    QString m_btnTop_normal;
    QString m_btnTop_handled;
};

#endif // MAINWINDOW_HPP
