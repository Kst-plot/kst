include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_fitlorentzian_weighted)
LIBS += -l$$kstlib(gsl)

SOURCES += \
    fitlorentzian_weighted.cpp

HEADERS += \
    fitlorentzian_weighted.h

FORMS += fitlorentzian_weightedconfig.ui
