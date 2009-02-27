include($$PWD/../../../../../kst.pri)
include($$PWD/../../../../../dataobjectplugin.pri)

TARGET = kstplugin_linearinterpolation
LIBS += -lgsl

SOURCES += \
    linear.cpp

HEADERS += \
    linear.h

FORMS += linearconfig.ui
