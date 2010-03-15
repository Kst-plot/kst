include(../../../plugins_subsub.pri)

TARGET = $$qtLibraryTarget(kstplugin_polynomialinterpolation)
LIBS += -lgsl

SOURCES += \
    polynomial.cpp

HEADERS += \
    polynomial.h

FORMS += polynomialconfig.ui
