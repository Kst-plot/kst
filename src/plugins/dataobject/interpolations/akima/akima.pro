include(../../../plugins_subsub.pri)

TARGET = $$qtLibraryTarget(kstplugin_akima)
LIBS += -lgsl

SOURCES += \
    akima.cpp

HEADERS += \
    akima.h

FORMS += akimaconfig.ui
