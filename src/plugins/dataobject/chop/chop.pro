include($$PWD/../../../../kst.pri)
include($$PWD/../../../../dataobjectplugin.pri)

TARGET = $$qtLibraryTarget(kstplugin_chop)

SOURCES += \
    chop.cpp

HEADERS += \
    chop.h

FORMS += chopconfig.ui
