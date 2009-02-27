include($$PWD/../../../../kst.pri)
include($$PWD/../../../../dataobjectplugin.pri)

TARGET = kstplugin_fitgaussian_weighted
LIBS += -lgsl

SOURCES += \
    fitgaussian_weighted.cpp

HEADERS += \
    fitgaussian_weighted.h

FORMS += fitgaussian_weightedconfig.ui
