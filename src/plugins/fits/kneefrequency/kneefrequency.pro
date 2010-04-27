include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_fitkneefrequency)
LIBS += -lgsl

SOURCES += \
    fitkneefrequency.cpp

HEADERS += \
    fitkneefrequency.h

FORMS += fitkneefrequencyconfig.ui
