TOPOUT_REL=../../..
include($$PWD/$$TOPOUT_REL/kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$kstlib(kst2data_dirfilesource)

INCLUDEPATH += $$OUTPUT_DIR/src/datasources/dirfilesource/tmp

LIBS += -lgetdata++
win32:LIBS += -lgetdata
macx:LIBS += -lgetdata -lz -lbz2

SOURCES += \
    dirfilesource.cpp

HEADERS += \
    dirfilesource.h
