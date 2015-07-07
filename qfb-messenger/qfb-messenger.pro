TEMPLATE = lib
CONFIG += staticlib
TARGET = lib/qfb-messenger

QT += network
QT -= gui

HEADERS += src/qfb-messenger.h
SOURCES += src/qfb-messenger.cpp
