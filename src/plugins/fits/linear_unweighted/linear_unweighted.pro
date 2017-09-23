include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_fitlinear_unweighted)
LIBS += -l$$kstlib(gsl)

SOURCES += \
    fitlinear_unweighted.cpp

HEADERS += \
    fitlinear_unweighted.h

FORMS += fitlinear_unweightedconfig.ui
