include(../../plugins_sub.pri)

TARGET = $$qtLibraryTarget(kstplugin_fitexponential_weighted)
LIBS += -lgsl

SOURCES += \
    fitexponential_weighted.cpp

HEADERS += \
    fitexponential_weighted.h

FORMS += fitexponential_weightedconfig.ui
