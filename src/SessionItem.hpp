#ifndef SESSIONITEM_HPP
#define SESSIONITEM_HPP

#include <QWidget>

namespace Ui {
class SessionItem;
}

class SessionItem : public QWidget
{
    Q_OBJECT

public:
    explicit SessionItem(QWidget *parent = 0);
    ~SessionItem();

private:
    Ui::SessionItem *ui;
};

#endif // SESSIONITEM_HPP
