include($$PWD/../../../../kst.pri)
include($$PWD/../../../../dataobjectplugin.pri)

TARGET = $$qtLibraryTarget(kstplugin_fitgradient_unweighted)
LIBS += -lgsl

SOURCES += \
    fitgradient_unweighted.cpp

HEADERS += \
    fitgradient_unweighted.h

FORMS += fitgradient_unweightedconfig.ui
