include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_filterbutterworthhighpass)
LIBS += -lgsl

SOURCES += \
    butterworth_highpass.cpp

HEADERS += \
    butterworth_highpass.h

FORMS += filterbutterworthhighpassconfig.ui
