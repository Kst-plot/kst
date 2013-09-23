TOPOUT_REL=../../..
include($$PWD/$$TOPOUT_REL/kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$kstlib(kst2data_sourcelist)
INCLUDEPATH += $$OUTPUT_DIR/src/datasources/sourcelist/tmp

SOURCES += \
    sourcelist.cpp

HEADERS += \
    sourcelist.h
