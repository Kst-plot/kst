TEMPLATE = subdirs
CONFIG += ordered qt thread

SUBDIRS += \
    src/libkst \
    src/libkstmath \
    src/datasources \
    src/widgets \
    src/libkstapp \
    src/kst

!macx:SUBDIRS += \
    src/d2asc \
    src/plugins \
    test

!win32-msvc*:SUBDIRS += tests/dirfile_maker
