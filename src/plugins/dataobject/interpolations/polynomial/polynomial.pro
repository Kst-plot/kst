include(../../../plugins_subsub.pri)

TARGET = $$kstlib(kstplugin_polynomialinterpolation)
LIBS += -l$$kstlib(gsl)

SOURCES += \
    polynomial.cpp

HEADERS += \
    polynomial.h

FORMS += polynomialconfig.ui
