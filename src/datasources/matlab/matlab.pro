TOPOUT_REL=../../..
include($$PWD/$$TOPOUT_REL/kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$kstlib(kst2data_matlabio)
INCLUDEPATH += $$OUTPUT_DIR/src/datasources/matlab/tmp

SOURCES += \
    matlab.cpp

HEADERS += \
    matlab.h
