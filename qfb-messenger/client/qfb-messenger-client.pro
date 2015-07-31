TEMPLATE = subdirs
SUBDIRS = common basic fb ../

common.depends = ../
basic.depends = common
fb.depends = common
