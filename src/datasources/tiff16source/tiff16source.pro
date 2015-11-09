TOPOUT_REL=../../..
include($$PWD/$$TOPOUT_REL/kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$kstlib(kst2data_tiff16source)
INCLUDEPATH += $$OUTPUT_DIR/src/datasources/tiff16source/tmp

SOURCES += \
    tiff16source.cpp

HEADERS += \
    tiff16source.h
