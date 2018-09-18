#include "Context.hpp"
#include "ui_Context.h"

Context::Context(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Context)
{
    ui->setupUi(this);
    QObject::connect(this->ui->pushButton_2, &QPushButton::clicked, this, [this](){
        auto text = this->ui->textEdit->toPlainText();
        auto item = new QListWidgetItem();
        item->setText(text);
        this->ui->ctxListWnd->addItem(item);

    });
}

Context::~Context()
{
    delete ui;
}
