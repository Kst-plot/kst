include(../../../plugins_subsub.pri)

TARGET = $$kstlib(kstplugin_deconvolve)
LIBS += -lgsl

SOURCES += \
    deconvolve.cpp

HEADERS += \
    deconvolve.h

FORMS += deconvolveconfig.ui
