TOPOUT_REL=../../..
include($$PWD/$$TOPOUT_REL/kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$kstlib(kst2data_fitsimage)

INCLUDEPATH += $$OUTPUT_DIR/src/datasources/fitsimage/tmp

LIBS += -lcfitsio

SOURCES += \
    fitsimage.cpp

HEADERS += \
    fitsimage.h
