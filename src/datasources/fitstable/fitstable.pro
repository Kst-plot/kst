TOPOUT_REL=../../..
include($$PWD/$$TOPOUT_REL/kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$kstlib(kst2data_fitstable)
INCLUDEPATH += $$OUTPUT_DIR/src/datasources/fitstable/tmp

SOURCES += \
    fitstable.cpp

HEADERS += \
    fitstable.h
