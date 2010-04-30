include(../../../plugins_subsub.pri)

TARGET = $$kstlib(kstplugin_linearinterpolation)
LIBS += -l$$kstlib(gsl)

SOURCES += \
    linear.cpp

HEADERS += \
    linear.h

FORMS += linearconfig.ui
