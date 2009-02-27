include($$PWD/../../../../kst.pri)
include($$PWD/../../../../dataobjectplugin.pri)

TARGET = kstplugin_fitgaussian_unweighted
LIBS += -lgsl

SOURCES += \
    fitgaussian_unweighted.cpp

HEADERS += \
    fitgaussian_unweighted.h

FORMS += fitgaussian_unweightedconfig.ui
