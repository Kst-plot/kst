include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_fitgradient_unweighted)
LIBS += -lgsl

SOURCES += \
    fitgradient_unweighted.cpp

HEADERS += \
    fitgradient_unweighted.h

FORMS += fitgradient_unweightedconfig.ui
