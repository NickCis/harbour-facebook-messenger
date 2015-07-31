TEMPLATE = subdirs
SUBDIRS = common basic fb ../qfb-messenger.pro

common.depends = ../qfb-messenger.pro
basic.depends = common
fb.depends = common
