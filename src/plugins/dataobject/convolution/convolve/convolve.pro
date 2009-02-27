include($$PWD/../../../../../kst.pri)
include($$PWD/../../../../../dataobjectplugin.pri)

TARGET = kstplugin_convolve
LIBS += -lgsl

SOURCES += \
    convolve.cpp

HEADERS += \
    convolve.h

FORMS += convolveconfig.ui
