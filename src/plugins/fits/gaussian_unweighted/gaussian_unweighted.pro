include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_fitgaussian_unweighted)
LIBS += -l$$kstlib(gsl)

SOURCES += \
    fitgaussian_unweighted.cpp

HEADERS += \
    fitgaussian_unweighted.h

FORMS += fitgaussian_unweightedconfig.ui
