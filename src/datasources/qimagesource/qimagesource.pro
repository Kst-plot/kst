TOPOUT_REL=../../..
include($$PWD/$$TOPOUT_REL/kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$kstlib(kst2data_qimagesource)
INCLUDEPATH += $$OUTPUT_DIR/src/datasources/qimagesource/tmp

SOURCES += \
    qimagesource.cpp

HEADERS += \
    qimagesource.h
