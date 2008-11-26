include($$PWD/../../../kst.pri)

QT += xml qt3support

TEMPLATE = lib
CONFIG += plugin
OBJECTS_DIR = tmp
MOC_DIR = tmp
TARGET = kstdata_planckIDEF
DESTDIR = $$OUTPUT_DIR/plugin

INCLUDEPATH += \
    tmp \
    $$TOPLEVELDIR/src/libkst \
    $$OUTPUT_DIR/src/datasources/fitsimage/tmp

LIBS += -lkst -lcfitsio

SOURCES += \
    planckIDEF.cpp

HEADERS += \
    planckIDEF.h

FORMS += planckIDEFconfig.ui