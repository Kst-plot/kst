include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_filterbutterworthlowpass)
LIBS += -l$$kstlib(gsl) 
!win32:LIBS += -l$$kstlib(gsl)cblas

SOURCES += \
    butterworth_lowpass.cpp

HEADERS += \
    butterworth_lowpass.h

FORMS += filterbutterworthlowpassconfig.ui
