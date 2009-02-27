include($$PWD/../../../../../kst.pri)
include($$PWD/../../../../../dataobjectplugin.pri)

TARGET = kstplugin_csplineperiodic
LIBS += -lgsl

SOURCES += \
    csplineperiodic.cpp

HEADERS += \
    csplineperiodic.h

FORMS += csplineperiodicconfig.ui
