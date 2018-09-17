#include "MainWindow.hpp"
#include <QApplication>
#include <QDir>
#include <QDebug>


static void setStyle();
int main(int argc, char *argv[])
{

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication a(argc, argv);
    MainWindow w;
    setStyle();
    w.show();

    return a.exec();
}
void setStyle(){
    QString styleFile = QDir::currentPath() + QString(QStringLiteral("/style/main.qss"));
    QFile style(styleFile);
    if(!style.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << style.errorString();
    }
    qApp->setStyleSheet(style.readAll());
    style.close();
}
