include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_crossspectrum)

SOURCES += \
    fftsg_h.c \
    crossspectrum.cpp

HEADERS += \
    crossspectrum.h

FORMS += crossspectrumconfig.ui
