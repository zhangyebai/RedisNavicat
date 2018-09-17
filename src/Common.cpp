#include "Common.hpp"

#include <QFile>

Common::Common()
{

}
QString Common::read(const QString & path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return QString();
    }
    QString content = QString(file.readAll());
    file.close();
    return content;
}
