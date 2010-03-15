include(../../../plugins_subsub.pri)

TARGET = $$qtLibraryTarget(kstplugin_convolve)
LIBS += -lgsl

SOURCES += \
    convolve.cpp

HEADERS += \
    convolve.h

FORMS += convolveconfig.ui
