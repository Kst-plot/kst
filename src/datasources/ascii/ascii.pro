include($$PWD/../../../kst.pri)

QT += xml qt3support

TEMPLATE = lib
CONFIG += plugin
OBJECTS_DIR = tmp
MOC_DIR = tmp
TARGET = kstdata_ascii
DESTDIR = $$OUTPUT_DIR/plugin
CONFIG += debug

INCLUDEPATH += \
    tmp \
    $$TOPLEVELDIR/src/libkst \
    $$OUTPUT_DIR/src/datasources/ascii/tmp

LIBS += -lkst

SOURCES += \
    ascii.cpp

HEADERS += \
    ascii.h

FORMS += asciiconfig.ui
