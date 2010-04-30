include(../../../plugins_subsub.pri)

TARGET = $$kstlib(kstplugin_autocorrelation)
LIBS += -l$$kstlib(gsl)

SOURCES += \
    autocorrelation.cpp

HEADERS += \
    autocorrelation.h

FORMS += autocorrelationconfig.ui
