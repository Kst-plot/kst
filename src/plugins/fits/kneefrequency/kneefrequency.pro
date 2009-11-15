include($$PWD/../../../../kst.pri)
include($$PWD/../../../../dataobjectplugin.pri)

TARGET = $$qtLibraryTarget(kstplugin_fitkneefrequency)
LIBS += -lgsl

SOURCES += \
    fitkneefrequency.cpp

HEADERS += \
    fitkneefrequency.h

FORMS += fitkneefrequencyconfig.ui
