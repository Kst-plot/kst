include($$PWD/../../../kst.pri)

QT += xml qt3support

TEMPLATE = lib
CONFIG += plugin
OBJECTS_DIR = tmp
MOC_DIR = tmp
TARGET = kstdata_lfiio
DESTDIR = $$OUTPUT_DIR/plugin
CONFIG += debug

INCLUDEPATH += \
    tmp \
    $$TOPLEVELDIR/src/libkst \
    $$OUTPUT_DIR/src/datasources/lfiio/tmp

LIBS += -lkst -lcfitsio

SOURCES += \
    lfiio.cpp

HEADERS += \
    lfiio.h