include($$PWD/../../../kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$qtLibraryTarget(kst2data_fitsimage)

INCLUDEPATH += $$OUTPUT_DIR/src/datasources/fitsimage/tmp

LIBS += -lcfitsio

SOURCES += \
    fitsimage.cpp

HEADERS += \
    fitsimage.h
