include($$PWD/../../../../../kst.pri)
include($$PWD/../../../../../dataobjectplugin.pri)

TARGET = $$qtLibraryTarget(kstplugin_linearinterpolation)
LIBS += -lgsl

SOURCES += \
    linear.cpp

HEADERS += \
    linear.h

FORMS += linearconfig.ui
