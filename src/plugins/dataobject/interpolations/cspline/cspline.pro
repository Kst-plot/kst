include(../../../plugins_subsub.pri)

TARGET = $$kstlib(kstplugin_cspline)
LIBS += -l$$kstlib(gsl)

SOURCES += \
    cspline.cpp

HEADERS += \
    cspline.h

FORMS += csplineconfig.ui
