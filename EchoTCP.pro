TEMPLATE = app
CONFIG += console c++11 thread
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    Sockets/mastersocket.cpp \
    Sockets/socketwrapper.cpp \
    ioworker.cpp \
    tcplistener.cpp \
    Sockets/slavesocket.cpp

HEADERS += \
    Sockets/mastersocket.h \
    Sockets/socketwrapper.h \
    ioworker.h \
    tcplistener.h \
    Sockets/slavesocket.h
