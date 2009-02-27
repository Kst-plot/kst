include($$PWD/../../../../../kst.pri)
include($$PWD/../../../../../dataobjectplugin.pri)

TARGET = kstplugin_akima
LIBS += -lgsl

SOURCES += \
    akima.cpp

HEADERS += \
    akima.h

FORMS += akimaconfig.ui
