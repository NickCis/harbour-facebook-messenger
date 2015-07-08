TEMPLATE = lib
CONFIG += staticlib
DESTDIR = lib

QT += network
QT -= gui

HEADERS += src/qfb-messenger.h
SOURCES += src/qfb-messenger.cpp
