include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_fitpolynomial_weighted)
LIBS += -l$$kstlib(gsl)

SOURCES += \
    fitpolynomial_weighted.cpp

HEADERS += \
    fitpolynomial_weighted.h

FORMS += fitpolynomial_weightedconfig.ui
