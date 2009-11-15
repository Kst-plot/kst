include($$PWD/../../../../kst.pri)
include($$PWD/../../../../dataobjectplugin.pri)

TARGET = $$qtLibraryTarget(kstplugin_fitpolynomial_unweighted)
LIBS += -lgsl

SOURCES += \
    fitpolynomial_unweighted.cpp

HEADERS += \
    fitpolynomial_unweighted.h

FORMS += fitpolynomial_unweightedconfig.ui
