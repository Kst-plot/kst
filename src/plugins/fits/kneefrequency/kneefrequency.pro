include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_fitkneefrequency)
LIBS += -l$$kstlib(gsl)

SOURCES += \
    fitkneefrequency.cpp

HEADERS += \
    fitkneefrequency.h

FORMS += fitkneefrequencyconfig.ui
