TEMPLATE = app
DESTDIR = ../bin

CONFIG += console


INCLUDEPATH += $$PWD/../../src/
LIBS += -L../../lib/ -lqfb-messenger

QT += core network
QT -= gui

INCLUDEPATH += $$PWD/../common/src/
LIBS += -L../common -lcommon

HEADERS = src/fb_client.h
SOURCES =  src/fb_client.cpp src/main.cpp
