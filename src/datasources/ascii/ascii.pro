include($$PWD/../../../kst.pri)
include($$PWD/../../../datasourceplugin.pri)
TARGET = kstdata_ascii
INCLUDEPATH += $$OUTPUT_DIR/src/datasources/ascii/tmp
SOURCES += ascii.cpp
HEADERS += ascii.h \
    ../../libkst/kst_inf.h
FORMS += asciiconfig.ui
