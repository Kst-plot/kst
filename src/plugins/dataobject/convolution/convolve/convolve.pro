include(../../../plugins_subsub.pri)

TARGET = $$kstlib(kstplugin_convolve)
LIBS += -l$$kstlib(gsl)

SOURCES += \
    convolve.cpp

HEADERS += \
    convolve.h

FORMS += convolveconfig.ui
