include($$PWD/../../../../../kst.pri)
include($$PWD/../../../../../dataobjectplugin.pri)

TARGET = kstplugin_polynomialinterpolation
LIBS += -lgsl

SOURCES += \
    polynomial.cpp

HEADERS += \
    polynomial.h

FORMS += polynomialconfig.ui
