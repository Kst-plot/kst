include(../../../plugins_subsub.pri)

TARGET = $$qtLibraryTarget(kstplugin_csplineperiodic)
LIBS += -lgsl

SOURCES += \
    csplineperiodic.cpp

HEADERS += \
    csplineperiodic.h

FORMS += csplineperiodicconfig.ui
