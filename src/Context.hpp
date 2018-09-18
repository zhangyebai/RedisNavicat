#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <QWidget>
#include <QTextEdit>

namespace Ui {
class Context;
}

class Context : public QWidget
{
    Q_OBJECT

public:
    explicit Context(QWidget *parent = 0);
    ~Context();

private:
    Ui::Context *ui;
};

#endif // CONTEXT_HPP
