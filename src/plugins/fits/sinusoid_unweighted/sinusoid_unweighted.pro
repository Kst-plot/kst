include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_fitsinusoid_unweighted)
LIBS += -l$$kstlib(gsl)

SOURCES += \
    fitsinusoid_unweighted.cpp

HEADERS += \
    fitsinusoid_unweighted.h

FORMS += fitsinusoid_unweightedconfig.ui
