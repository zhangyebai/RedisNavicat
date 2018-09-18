#include "SessionItem.hpp"
#include "ui_SessionItem.h"

SessionItem::SessionItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SessionItem)
{
    ui->setupUi(this);
}

SessionItem::~SessionItem()
{
    delete ui;
}
