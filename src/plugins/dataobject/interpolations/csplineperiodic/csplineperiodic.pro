include(../../../plugins_subsub.pri)

TARGET = $$kstlib(kstplugin_csplineperiodic)
LIBS += -l$$kstlib(gsl)

SOURCES += \
    csplineperiodic.cpp

HEADERS += \
    csplineperiodic.h

FORMS += csplineperiodicconfig.ui
