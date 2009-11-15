include($$PWD/../../../../kst.pri)
include($$PWD/../../../../dataobjectplugin.pri)

TARGET = $$qtLibraryTarget(kstplugin_fitlinear_unweighted)
LIBS += -lgsl

SOURCES += \
    fitlinear_unweighted.cpp

HEADERS += \
    fitlinear_unweighted.h

FORMS += fitlinear_unweightedconfig.ui
