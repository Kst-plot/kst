include(../../../plugins_subsub.pri)

TARGET = $$kstlib(kstplugin_csplineperiodic)
LIBS += -lgsl

SOURCES += \
    csplineperiodic.cpp

HEADERS += \
    csplineperiodic.h

FORMS += csplineperiodicconfig.ui
