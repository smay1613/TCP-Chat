TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    Sockets/mastersocket.cpp \
    Sockets/socketwrapper.cpp \
    tcplistener.cpp \
    Sockets/slavesocket.cpp \
    ioserver.cpp \
    ioworker.cpp

HEADERS += \
    Sockets/mastersocket.h \
    Sockets/socketwrapper.h \
    tcplistener.h \
    Sockets/slavesocket.h \
    ioserver.h \
    ioworker.h
