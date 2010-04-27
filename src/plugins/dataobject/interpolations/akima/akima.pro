include(../../../plugins_subsub.pri)

TARGET = $$kstlib(kstplugin_akima)
LIBS += -lgsl

SOURCES += \
    akima.cpp

HEADERS += \
    akima.h

FORMS += akimaconfig.ui
