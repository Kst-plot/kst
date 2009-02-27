include($$PWD/../../../../kst.pri)
include($$PWD/../../../../dataobjectplugin.pri)

TARGET = kstplugin_noiseaddition
LIBS += -lgsl

SOURCES += \
    noiseaddition.cpp

HEADERS += \
    noiseaddition.h

FORMS += noiseadditionconfig.ui
