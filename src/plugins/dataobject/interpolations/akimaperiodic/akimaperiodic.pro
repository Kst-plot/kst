include($$PWD/../../../../../kst.pri)
include($$PWD/../../../../../dataobjectplugin.pri)

TARGET = $$qtLibraryTarget(kstplugin_akimaperiodic)

LIBS += -lgsl

SOURCES += \
    akimaperiodic.cpp

HEADERS += \
    akimaperiodic.h

FORMS += akimaperiodicconfig.ui
