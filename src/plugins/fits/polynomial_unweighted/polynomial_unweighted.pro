include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_fitpolynomial_unweighted)
LIBS += -l$$kstlib(gsl)

SOURCES += \
    fitpolynomial_unweighted.cpp

HEADERS += \
    fitpolynomial_unweighted.h

FORMS += fitpolynomial_unweightedconfig.ui
