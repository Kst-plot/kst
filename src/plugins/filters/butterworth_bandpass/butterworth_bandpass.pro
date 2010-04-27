include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_filterbutterworthbandpass)
LIBS += -lgsl

SOURCES += \
    butterworth_bandpass.cpp

HEADERS += \
    butterworth_bandpass.h

FORMS += filterbutterworthbandpassconfig.ui
