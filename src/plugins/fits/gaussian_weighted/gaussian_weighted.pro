include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_fitgaussian_weighted)
LIBS += -l$$kstlib(gsl)

SOURCES += \
    fitgaussian_weighted.cpp

HEADERS += \
    fitgaussian_weighted.h

FORMS += fitgaussian_weightedconfig.ui
