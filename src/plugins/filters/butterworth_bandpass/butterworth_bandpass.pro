include($$PWD/../../../../kst.pri)
include($$PWD/../../../../dataobjectplugin.pri)

TARGET = $$qtLibraryTarget(kstplugin_filterbutterworthbandpass)
LIBS += -lgsl

SOURCES += \
    butterworth_bandpass.cpp

HEADERS += \
    butterworth_bandpass.h

FORMS += filterbutterworthbandpassconfig.ui
