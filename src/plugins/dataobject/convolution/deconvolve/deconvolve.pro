include(../../../plugins_subsub.pri)

TARGET = $$kstlib(kstplugin_deconvolve)
LIBS += -l$$kstlib(gsl)

SOURCES += \
    deconvolve.cpp

HEADERS += \
    deconvolve.h

FORMS += deconvolveconfig.ui
