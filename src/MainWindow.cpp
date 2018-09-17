#include "MainWindow.hpp"
#include "ui_mainwindow.h"

//#define __CPP_REDIS_USE_CUSTOM_TCP_CLIENT


#include "Common.hpp"
#include "hpp/cpp_redis/cpp_redis"

#ifdef Q_OS_WIN
#include <WinSock2.h>
#include <Windows.h>
#endif

#include <QDebug>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->initMainWindow();
    this->initMainSignals();
#ifdef Q_OS_WIN
    WORD version = MAKEWORD(2, 2);
    WSADATA data = { 0 };

    if (WSAStartup(version, &data) != 0) {
        qDebug() << "windows network init failed.";
    }
#endif
    qDebug() << "start" ;
    cpp_redis::active_logger = std::unique_ptr<cpp_redis::logger>(new cpp_redis::logger(cpp_redis::logger::log_level::debug));
    cpp_redis::client client;
    client.connect("140.143.15.163", 6379, [=](const std::string& host, std::size_t port, cpp_redis::client::connect_state status){
        if (status == cpp_redis::client::connect_state::dropped) {
             qDebug() << "client disconnected from " << host.c_str() << ":" << port;
        }else{
            qDebug() << QString::number((int)status, 10) << host.c_str() << port;
        }
    });
     client.sync_commit();
    client.auth("123456", [](const cpp_redis::reply & reply){
        //qDebug() << reply.as_array();
    });
    client.sync_commit();
    client.get("tencent", [](const cpp_redis::reply & reply){
        qDebug() << reply.as_string().c_str();
    });
    client.sync_commit();

#ifdef _WIN32
    //WSACleanup();
#endif /* _WIN32 */
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::initMainWindow()
{
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint |
                         Qt::WindowSystemMenuHint |
                         Qt::WindowMaximizeButtonHint |
                         Qt::WindowMinimizeButtonHint);
    QString base_dir = QDir::currentPath();
    this->m_btnTop_normal = Common::read(base_dir + QString(QStringLiteral("/style/top.qss")));
    this->m_btnTop_handled = Common::read(base_dir + QString(QStringLiteral("/style/top-handled.qss")));
}

void MainWindow::initMainSignals()
{
    QObject::connect(this->ui->BtnClose, &QPushButton::clicked, this, [](){
        // quit之前需要释放某些资源
        QApplication::quit();
    });

    QObject::connect(this->ui->BtnTop, &QPushButton::clicked, this, [this](){
        if(this->m_wndTop){
            //取消窗口置顶
#ifdef Q_OS_WIN
            ::SetWindowPos((HWND)this->winId(), HWND_NOTOPMOST, this->pos().x(), this->pos().y(), width(), height(), SWP_SHOWWINDOW);
#endif
            this->m_wndTop = false;
            this->ui->BtnTop->setStyleSheet(this->m_btnTop_normal);
        }else{
            //窗口置顶
#ifdef Q_OS_WIN
            ::SetWindowPos((HWND)this->winId(), HWND_TOPMOST, this->pos().x(), this->pos().y(), width(), height(), SWP_SHOWWINDOW);
#endif
            this->m_wndTop = true;
            this->ui->BtnTop->setStyleSheet(this->m_btnTop_handled);
        }
    });

    QObject::connect(this->ui->BtnMax, &QPushButton::clicked, this, [this](){
        if(this->m_wndMax){
            this->showNormal();
            this->m_wndMax = false;
            return;
        }else{
            this->showMaximized();
            this->m_wndMax = true;
        }
    });
}
