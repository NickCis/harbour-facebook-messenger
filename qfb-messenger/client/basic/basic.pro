TEMPLATE = app
DESTDIR = ../bin

CONFIG += console

INCLUDEPATH += $$PWD/../common/src/
LIBS += -L../common -lcommon

INCLUDEPATH += $$PWD/../../src/
LIBS += -L../../lib/ -lqfb-messenger

QT += core network
QT -= gui

SOURCES = src/main.cpp src/basic_client.cpp
