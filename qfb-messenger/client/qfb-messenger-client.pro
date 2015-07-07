TEMPLATE = app
TARGET = bin/qfb-messenger-client
qfb-messenger-client.depends = ../qfb-messenger

LIBS += ../lib/libqfb-messenger.a

CONFIG += console

INCLUDEPATH += $$PWD/../src/

QT += core network
QT -= gui

HEADERS += src/console.h
SOURCES += src/main.cpp src/console.cpp
