TOPOUT_REL=../../..
include($$PWD/$$TOPOUT_REL/kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$kstlib(kst2data_netcdfsource)

INCLUDEPATH += $$OUTPUT_DIR/src/datasources/netcdf/tmp

LIBS += -l$$kstlib(netcdf_c++) -l$$kstlib(netcdf) 

SOURCES += \
    netcdfsource.cpp \
    netcdfplugin.cpp

HEADERS += \
    netcdfsource.h \
    netcdfplugin.h
