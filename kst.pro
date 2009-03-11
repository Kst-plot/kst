TEMPLATE = subdirs
CONFIG += ordered qt thread

SUBDIRS += \
    src/libkst \
    src/libkstmath \
    src/datasources \
    src/widgets \
    src/libkstapp \
    src/kst \
    src/d2asc \
    src/plugins

!win32:SUBDIRS += tests
