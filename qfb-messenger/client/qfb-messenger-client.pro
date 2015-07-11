TEMPLATE = app
DESTDIR = bin

LIBS += -L../lib/ -lqfb-messenger

CONFIG += console

INCLUDEPATH += $$PWD/../src/

QT += core network
QT -= gui

HEADERS += src/console.h src/basic_client.h
SOURCES += src/main.cpp src/console.cpp src/basic_client.cpp
