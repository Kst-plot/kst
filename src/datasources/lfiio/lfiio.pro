include($$PWD/../../../kst.pri)
include($$PWD/../../../datasourceplugin.pri)

TARGET = $$kstlib(kst2data_lfiio)

INCLUDEPATH += $$OUTPUT_DIR/src/datasources/lfiio/tmp

LIBS += -lcfitsio

SOURCES += \
    lfiio.cpp

HEADERS += \
    lfiio.h
