TEMPLATE = lib
CONFIG += staticlib console

QT += network
QT -= gui

HEADERS += src/console.h
SOURCES +=  src/console.cpp

INCLUDEPATH += $$PWD/../../src/
