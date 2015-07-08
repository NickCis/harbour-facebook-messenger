TEMPLATE = app
DESTDIR = bin

LIBS += -L../lib/ -lqfb-messenger

CONFIG += console

INCLUDEPATH += $$PWD/../src/

QT += core network
QT -= gui

HEADERS += src/console.h
SOURCES += src/main.cpp src/console.cpp
