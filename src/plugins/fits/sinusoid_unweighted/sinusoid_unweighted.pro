include(../../plugins_sub.pri)

TARGET = $$qtLibraryTarget(kstplugin_fitsinusoid_unweighted)
LIBS += -lgsl

SOURCES += \
    fitsinusoid_unweighted.cpp

HEADERS += \
    fitsinusoid_unweighted.h

FORMS += fitsinusoid_unweightedconfig.ui
