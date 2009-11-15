include($$PWD/../../../../kst.pri)
include($$PWD/../../../../dataobjectplugin.pri)

TARGET = $$qtLibraryTarget(kstplugin_fitlorentzian_weighted)
LIBS += -lgsl

SOURCES += \
    fitlorentzian_weighted.cpp

HEADERS += \
    fitlorentzian_weighted.h

FORMS += fitlorentzian_weightedconfig.ui
