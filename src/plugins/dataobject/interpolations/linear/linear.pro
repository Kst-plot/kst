include(../../../plugins_subsub.pri)

TARGET = $$kstlib(kstplugin_linearinterpolation)
LIBS += -lgsl

SOURCES += \
    linear.cpp

HEADERS += \
    linear.h

FORMS += linearconfig.ui
