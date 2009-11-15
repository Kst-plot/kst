include($$PWD/../../../kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$qtLibraryTarget(kst2data_qimagesource)
INCLUDEPATH += $$OUTPUT_DIR/src/datasources/qimagesource/tmp

SOURCES += \
    qimagesource.cpp

HEADERS += \
    qimagesource.h
