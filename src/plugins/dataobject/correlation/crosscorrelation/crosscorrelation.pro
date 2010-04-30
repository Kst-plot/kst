include(../../../plugins_subsub.pri)

TARGET = $$kstlib(kstplugin_crosscorrelation)

LIBS += -l$$kstlib(gsl)

SOURCES += \
    crosscorrelation.cpp

HEADERS += \
    crosscorrelation.h

FORMS += crosscorrelationconfig.ui
