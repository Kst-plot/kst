TOPOUT_REL=../../..
include($$PWD/$$TOPOUT_REL/kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$qtLibraryTarget(kst2data_ascii)
INCLUDEPATH += $$OUTPUT_DIR/src/datasources/ascii/tmp
SOURCES += ascii.cpp
win32:SOURCES += kst_atof.cpp
HEADERS += ascii.h \
    ../../libkst/kst_inf.h
win32:HEADERS += kst_atof.h
FORMS += asciiconfig.ui
