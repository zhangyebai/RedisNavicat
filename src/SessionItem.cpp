#include "SessionItem.hpp"
#include "ui_SessionItem.h"
#include <QPixmap>
#include <QDebug>

SessionItem::SessionItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SessionItem)
{
    ui->setupUi(this);
    auto size = this->ui->sessionLabel->size();
    qDebug() << size;
    this->ui->sessionLabel->setPixmap(QPixmap("./resource/image/logo.png").scaled(size));
    this->ui->nameLabel->setText("redis on localhost");
    this->ui->timeLabel->setText("12:03");
    this->ui->extraLabel->setText("tencent cloud");
}

SessionItem::~SessionItem()
{
    delete ui;
}
