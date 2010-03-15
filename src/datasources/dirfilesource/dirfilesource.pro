TOPOUT_REL=../../..
include($$PWD/$$TOPOUT_REL/kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$qtLibraryTarget(kst2data_dirfilesource)

INCLUDEPATH += $$OUTPUT_DIR/src/datasources/dirfilesource/tmp

LIBS += -lgetdata++

SOURCES += \
    dirfilesource.cpp

HEADERS += \
    dirfilesource.h
