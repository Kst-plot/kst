include(../../../plugins_subsub.pri)

TARGET = $$kstlib(kstplugin_akimaperiodic)

LIBS += -lgsl

SOURCES += \
    akimaperiodic.cpp

HEADERS += \
    akimaperiodic.h

FORMS += akimaperiodicconfig.ui
