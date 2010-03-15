include(../../plugins_sub.pri)

TARGET = $$qtLibraryTarget(kstplugin_fitpolynomial_weighted)
LIBS += -lgsl

SOURCES += \
    fitpolynomial_weighted.cpp

HEADERS += \
    fitpolynomial_weighted.h

FORMS += fitpolynomial_weightedconfig.ui
