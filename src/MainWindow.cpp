#include "MainWindow.hpp"
#include "ui_mainwindow.h"

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
}

void MainWindow::initMainSignals()
{
    QObject::connect(this->ui->BtnClose, &QPushButton::clicked, this, [](){
        QApplication::quit();
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
