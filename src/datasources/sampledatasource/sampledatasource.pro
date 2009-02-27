include($$PWD/../../../kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = kstdata_sampledatasource
INCLUDEPATH += $$OUTPUT_DIR/src/datasources/sampledatasource/tmp

SOURCES += \
    sampledatasource.cpp

HEADERS += \
    sampledatasource.h