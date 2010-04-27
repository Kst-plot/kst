include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_filterbutterworthlowpass)
LIBS += -lgsl -lgslcblas

SOURCES += \
    butterworth_lowpass.cpp

HEADERS += \
    butterworth_lowpass.h

FORMS += filterbutterworthlowpassconfig.ui
