include($$PWD/../../../kst.pri)

QT += xml qt3support

TEMPLATE = lib
CONFIG += plugin
OBJECTS_DIR = tmp
MOC_DIR = tmp
TARGET = kstdata_healpix
DESTDIR = $$OUTPUT_DIR/plugin

INCLUDEPATH += \
    tmp \
    $$TOPLEVELDIR/src/libkst \
    $$OUTPUT_DIR/src/datasources/fitsimage/tmp

LIBS += -lkst -lcfitsio

SOURCES += \
    healpix.cpp \
    healpix_tools_fits.cpp \
    healpix_tools_pix.cpp \
    healpix_tools_proj.cpp

HEADERS += \
    healpix.h \
    healpix_tools.h

FORMS += healpixconfig.ui