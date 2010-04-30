include(../../../plugins_subsub.pri)

TARGET = $$kstlib(kstplugin_akima)
LIBS += -l$$kstlib(gsl)

SOURCES += \
    akima.cpp

HEADERS += \
    akima.h

FORMS += akimaconfig.ui
