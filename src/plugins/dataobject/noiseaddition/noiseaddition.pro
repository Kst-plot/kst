include(../../plugins_sub.pri)

TARGET = $$qtLibraryTarget(kstplugin_noiseaddition)
LIBS += -lgsl

SOURCES += \
    noiseaddition.cpp

HEADERS += \
    noiseaddition.h

FORMS += noiseadditionconfig.ui
