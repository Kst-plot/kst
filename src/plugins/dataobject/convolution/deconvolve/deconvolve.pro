include(../../../plugins_subsub.pri)

TARGET = $$qtLibraryTarget(kstplugin_deconvolve)
LIBS += -lgsl

SOURCES += \
    deconvolve.cpp

HEADERS += \
    deconvolve.h

FORMS += deconvolveconfig.ui
