TEMPLATE = app
DESTDIR = ../bin

CONFIG += console

INCLUDEPATH += $$PWD/../common/src/
LIBS += -L../common -lcommon

INCLUDEPATH += $$PWD/../../src/
LIBS += -L../../lib/ -lqfb-messenger

QT += core network
QT -= gui

SOURCES = src/fb_client.cpp src/main.cpp
