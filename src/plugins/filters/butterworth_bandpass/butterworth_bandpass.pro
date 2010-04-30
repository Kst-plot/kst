include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_filterbutterworthbandpass)
LIBS += -l$$kstlib(gsl)

SOURCES += \
    butterworth_bandpass.cpp

HEADERS += \
    butterworth_bandpass.h

FORMS += filterbutterworthbandpassconfig.ui
