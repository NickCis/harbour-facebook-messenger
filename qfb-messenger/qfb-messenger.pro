TEMPLATE = lib
CONFIG += staticlib
DESTDIR = lib

QT += network
QT -= gui

HEADERS += src/qfb-messenger.h src/network_manager.h src/network_reply_handler.h src/network_reply_response.h src/network_reply_reader.h src/q_network_reply_helper.h
SOURCES += src/qfb-messenger.cpp src/network_manager.cpp src/network_reply_handler.cpp src/network_reply_response.cpp src/network_reply_reader.cpp src/q_network_reply_helper.cpp
