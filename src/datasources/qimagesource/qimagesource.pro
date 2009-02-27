include($$PWD/../../../kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = kstdata_qimagesource
INCLUDEPATH += $$OUTPUT_DIR/src/datasources/qimagesource/tmp

SOURCES += \
    qimagesource.cpp

HEADERS += \
    qimagesource.h