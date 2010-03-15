TOPOUT_REL=../../..
include($$PWD/$$TOPOUT_REL/kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$qtLibraryTarget(kst2data_sampledatasource)
INCLUDEPATH += $$OUTPUT_DIR/src/datasources/sampledatasource/tmp

SOURCES += \
    sampledatasource.cpp

HEADERS += \
    sampledatasource.h
