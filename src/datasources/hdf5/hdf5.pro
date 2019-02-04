TOPOUT_REL=../../..
include($$PWD/$$TOPOUT_REL/kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$kstlib(kst2data_hdf5)
INCLUDEPATH += $$OUTPUT_DIR/src/datasources/hdf5/tmp

LIBS += -lhdf5_cpp -lhdf5_hl_cpp -lhdf5 -lhdf5_hl

SOURCES += \
    hdf5.cpp

HEADERS += \
    hdf5.h
