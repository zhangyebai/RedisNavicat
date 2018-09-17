#-------------------------------------------------
#
# Project created by QtCreator 2018-09-17T15:16:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RedisNavicat
TEMPLATE = app
RC_ICONS = redis.ico


win32{
    QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"
    INCLUDEPATH += "C:/Program Files (x86)/Windows Kits/10/Include/10.0.10240.0/ucrt"
    LIBS += -lUser32
    contains(QMAKE_HOST.arch, x86_64){
        LIBS += -L"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.10240.0/ucrt/x64"
    }else{
        LIBS += -L"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.10240.0/ucrt/x86"
    }
}



# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        src\main.cpp \
        src\MainWindow.cpp \
    src/Common.cpp

HEADERS += \
        src\MainWindow.hpp \
    src/Common.hpp

FORMS += \
        src\mainwindow.ui


CONFIG += c++11
