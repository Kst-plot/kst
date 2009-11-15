include($$PWD/../../../../kst.pri)
include($$PWD/../../../../dataobjectplugin.pri)

TARGET = $$qtLibraryTarget(kstplugin_periodogram)

SOURCES += \
    periodogram.cpp

HEADERS += \
    periodogram.h

FORMS += periodogramconfig.ui
