include(../../../plugins_subsub.pri)

TARGET = $$qtLibraryTarget(kstplugin_autocorrelation)
LIBS += -lgsl

SOURCES += \
    autocorrelation.cpp

HEADERS += \
    autocorrelation.h

FORMS += autocorrelationconfig.ui
