TEMPLATE = lib
CONFIG += staticlib
DESTDIR = lib

QT += network
QT -= gui

HEADERS += src/qfb-network.h src/qfb-messenger.h src/qfb-storage.h src/dummy_storage.h src/network_manager.h src/network_reply_handler.h src/network_reply_response.h src/network_reply_reader.h src/q_network_reply_helper.h
SOURCES += src/qfb-network.cpp src/qfb-messenger.cpp src/network_manager.cpp src/dummy_storage.cpp src/network_reply_handler.cpp src/network_reply_response.cpp src/network_reply_reader.cpp src/q_network_reply_helper.cpp
