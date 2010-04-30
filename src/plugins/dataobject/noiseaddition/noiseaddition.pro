include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_noiseaddition)
LIBS += -l$$kstlib(gsl)

SOURCES += \
    noiseaddition.cpp

HEADERS += \
    noiseaddition.h

FORMS += noiseadditionconfig.ui
