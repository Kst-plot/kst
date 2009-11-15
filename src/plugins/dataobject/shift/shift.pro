include($$PWD/../../../../kst.pri)
include($$PWD/../../../../dataobjectplugin.pri)

TARGET = $$qtLibraryTarget(kstplugin_shift)

SOURCES += \
    shift.cpp

HEADERS += \
    shift.h

FORMS += shiftconfig.ui
