include($$PWD/../../../../../kst.pri)
include($$PWD/../../../../../dataobjectplugin.pri)

TARGET = kstplugin_cspline
LIBS += -lgsl

SOURCES += \
    cspline.cpp

HEADERS += \
    cspline.h

FORMS += csplineconfig.ui
