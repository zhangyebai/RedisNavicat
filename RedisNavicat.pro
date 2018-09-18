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
    src/Common.cpp \
    src/main.cpp \
    src/MainWindow.cpp \
    src/cpp/cpp_redis/builders/array_builder.cpp \
    src/cpp/cpp_redis/builders/builders_factory.cpp \
    src/cpp/cpp_redis/builders/bulk_string_builder.cpp \
    src/cpp/cpp_redis/builders/error_builder.cpp \
    src/cpp/cpp_redis/builders/integer_builder.cpp \
    src/cpp/cpp_redis/builders/reply_builder.cpp \
    src/cpp/cpp_redis/builders/simple_string_builder.cpp \
    src/cpp/cpp_redis/core/client.cpp \
    src/cpp/cpp_redis/core/reply.cpp \
    src/cpp/cpp_redis/core/sentinel.cpp \
    src/cpp/cpp_redis/core/subscriber.cpp \
    src/cpp/cpp_redis/misc/logger.cpp \
    src/cpp/cpp_redis/network/redis_connection.cpp \
    src/cpp/cpp_redis/network/tcp_client.cpp \
    src/cpp/tacopie/network/common/tcp_socket.cpp \
    src/cpp/tacopie/network/unix/unix_self_pipe.cpp \
    src/cpp/tacopie/network/unix/unix_tcp_socket.cpp \
    src/cpp/tacopie/network/windows/windows_self_pipe.cpp \
    src/cpp/tacopie/network/windows/windows_tcp_socket.cpp \
    src/cpp/tacopie/network/io_service.cpp \
    src/cpp/tacopie/network/tcp_server.cpp \
    src/cpp/tacopie/utils/error.cpp \
    src/cpp/tacopie/utils/thread_pool.cpp \
    src/cpp/tacopie/utils/tacopie_logger.cpp \
    src/cpp/tacopie/network/tacopie_tcp_client.cpp \
    src/SessionItem.cpp \
    src/Context.cpp
    


HEADERS += \
    src/Common.hpp \
    src/MainWindow.hpp \
    src/hpp/cpp_redis/builders/array_builder.hpp \
    src/hpp/cpp_redis/builders/builder_iface.hpp \
    src/hpp/cpp_redis/builders/builders_factory.hpp \
    src/hpp/cpp_redis/builders/bulk_string_builder.hpp \
    src/hpp/cpp_redis/builders/error_builder.hpp \
    src/hpp/cpp_redis/builders/integer_builder.hpp \
    src/hpp/cpp_redis/builders/reply_builder.hpp \
    src/hpp/cpp_redis/builders/simple_string_builder.hpp \
    src/hpp/cpp_redis/core/client.hpp \
    src/hpp/cpp_redis/core/reply.hpp \
    src/hpp/cpp_redis/core/sentinel.hpp \
    src/hpp/cpp_redis/core/subscriber.hpp \
    src/hpp/cpp_redis/helpers/variadic_template.hpp \
    src/hpp/cpp_redis/impl/client.ipp \
    src/hpp/cpp_redis/misc/error.hpp \
    src/hpp/cpp_redis/misc/logger.hpp \
    src/hpp/cpp_redis/misc/macro.hpp \
    src/hpp/cpp_redis/network/redis_connection.hpp \
    src/hpp/cpp_redis/network/tcp_client.hpp \
    src/hpp/cpp_redis/network/tcp_client_iface.hpp \
    src/hpp/cpp_redis/cpp_redis \
    src/hpp/tacopie/network/io_service.hpp \
    src/hpp/tacopie/network/self_pipe.hpp \
    src/hpp/tacopie/network/tcp_server.hpp \
    src/hpp/tacopie/network/tcp_socket.hpp \
    src/hpp/tacopie/utils/error.hpp \
    src/hpp/tacopie/utils/thread_pool.hpp \
    src/hpp/tacopie/utils/typedefs.hpp \
    src/hpp/tacopie/tacopie \
    src/hpp/tacopie/utils/tacopie_logger.hpp \
    src/hpp/tacopie/network/tacopie_tcp_client.hpp \
    src/SessionItem.hpp \
    src/Context.hpp \
    src/time.hpp
    


FORMS += \
        src\mainwindow.ui \
    src/SessionItem.ui \
    src/Context.ui


CONFIG += c++14
CONFIG += no_batch
