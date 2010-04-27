include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_fitsinusoid_unweighted)
LIBS += -lgsl

SOURCES += \
    fitsinusoid_unweighted.cpp

HEADERS += \
    fitsinusoid_unweighted.h

FORMS += fitsinusoid_unweightedconfig.ui
