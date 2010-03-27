TOPOUT_REL=../../..
include($$PWD/$$TOPOUT_REL/kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$qtLibraryTarget(kst2data_netcdfsource)

INCLUDEPATH += $$OUTPUT_DIR/src/datasources/netcdf/tmp

LIBS += -lnetcdf

SOURCES += \
    kstnetcdf.cpp

HEADERS += \
    kstnetcdf.h
