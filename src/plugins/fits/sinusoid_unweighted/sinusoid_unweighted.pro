include($$PWD/../../../../kst.pri)
include($$PWD/../../../../dataobjectplugin.pri)

TARGET = kstplugin_fitsinusoid_unweighted
LIBS += -lgsl

SOURCES += \
    fitsinusoid_unweighted.cpp

HEADERS += \
    fitsinusoid_unweighted.h

FORMS += fitsinusoid_unweightedconfig.ui
