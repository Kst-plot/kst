include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_fitsinusoid_weighted)
LIBS += -lgsl

SOURCES += \
    fitsinusoid_weighted.cpp

HEADERS += \
    fitsinusoid_weighted.h

FORMS += fitsinusoid_weightedconfig.ui
