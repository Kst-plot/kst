include(../../../plugins_subsub.pri)

TARGET = $$qtLibraryTarget(kstplugin_crosscorrelation)

LIBS += -lgsl

SOURCES += \
    crosscorrelation.cpp

HEADERS += \
    crosscorrelation.h

FORMS += crosscorrelationconfig.ui
