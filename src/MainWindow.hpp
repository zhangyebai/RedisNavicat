﻿#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>


#include "Common.hpp"
#include "hpp/cpp_redis/cpp_redis"
#include "SessionItem.hpp"
#include "Context.hpp"
#include <string>
#include <ctime>


class Session{


public:
    std::time_t m_latest;   //最后会话时间
};

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
    void onStart();
    void onClose();
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
