include($$PWD/../../../../../kst.pri)
include($$PWD/../../../../../dataobjectplugin.pri)

TARGET = $$qtLibraryTarget(kstplugin_convolve)
LIBS += -lgsl

SOURCES += \
    convolve.cpp

HEADERS += \
    convolve.h

FORMS += convolveconfig.ui
