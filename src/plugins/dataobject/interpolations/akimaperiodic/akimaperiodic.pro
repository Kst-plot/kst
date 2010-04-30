include(../../../plugins_subsub.pri)

TARGET = $$kstlib(kstplugin_akimaperiodic)

LIBS += -l$$kstlib(gsl)

SOURCES += \
    akimaperiodic.cpp

HEADERS += \
    akimaperiodic.h

FORMS += akimaperiodicconfig.ui
