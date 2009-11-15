include($$PWD/../../../../kst.pri)
include($$PWD/../../../../dataobjectplugin.pri)

TARGET = $$qtLibraryTarget(kstplugin_crossspectrum)

SOURCES += \
    fftsg_h.c \
    crossspectrum.cpp

HEADERS += \
    crossspectrum.h

FORMS += crossspectrumconfig.ui
