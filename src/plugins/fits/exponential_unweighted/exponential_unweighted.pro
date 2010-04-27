include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_fitexponential_unweighted)
LIBS += -lgsl

SOURCES += \
    fitexponential_unweighted.cpp

HEADERS += \
    fitexponential_unweighted.h

FORMS += fitexponential_unweightedconfig.ui
