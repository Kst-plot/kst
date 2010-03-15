include(../../plugins_sub.pri)

TARGET = $$qtLibraryTarget(kstplugin_fitlinear_weighted)
LIBS += -lgsl

SOURCES += \
    fitlinear_weighted.cpp

HEADERS += \
    fitlinear_weighted.h

FORMS += fitlinear_weightedconfig.ui
