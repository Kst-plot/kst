include(../../../plugins_subsub.pri)

TARGET = $$kstlib(kstplugin_convolve)
LIBS += -lgsl

SOURCES += \
    convolve.cpp

HEADERS += \
    convolve.h

FORMS += convolveconfig.ui
