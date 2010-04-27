include(../../../plugins_subsub.pri)

TARGET = $$kstlib(kstplugin_crosscorrelation)

LIBS += -lgsl

SOURCES += \
    crosscorrelation.cpp

HEADERS += \
    crosscorrelation.h

FORMS += crosscorrelationconfig.ui
