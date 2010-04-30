include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_fitlinear_weighted)
LIBS += -l$$kstlib(gsl)

SOURCES += \
    fitlinear_weighted.cpp

HEADERS += \
    fitlinear_weighted.h

FORMS += fitlinear_weightedconfig.ui
