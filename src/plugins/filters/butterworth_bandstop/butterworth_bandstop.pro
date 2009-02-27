include($$PWD/../../../../kst.pri)
include($$PWD/../../../../dataobjectplugin.pri)

TARGET = kstplugin_filterbutterworthbandstop
LIBS += -lgsl

SOURCES += \
    butterworth_bandstop.cpp

HEADERS += \
    butterworth_bandstop.h

FORMS += filterbutterworthbandstopconfig.ui
