#include "MainWindow.hpp"
#include "ui_mainwindow.h"


#include "Common.hpp"

#ifdef Q_OS_WIN
#include "Windows.h"
#endif

#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->initMainWindow();
    this->initMainSignals();
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
