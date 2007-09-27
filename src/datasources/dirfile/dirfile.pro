include($$PWD/../../../kst.pri)

QT += xml qt3support

TEMPLATE = lib
CONFIG += plugin
OBJECTS_DIR = tmp
MOC_DIR = tmp
TARGET = kstdata_dirfile
DESTDIR = $$OUTPUT_DIR/plugin

INCLUDEPATH += \
    tmp \
    $$TOPLEVELDIR/src/libkst \
    $$OUTPUT_DIR/src/datasources/dirfile/tmp

LIBS += -lkst

SOURCES += \
    dirfile.cpp \
    getdata.c

HEADERS += \
    dirfile.h \
    getdata.h \
    getdata_struct.h
