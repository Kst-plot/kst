include($$PWD/../../../../kst.pri)
include($$PWD/../../../../dataobjectplugin.pri)

TARGET = $$qtLibraryTarget(kstplugin_bin)

SOURCES += \
    bin.cpp

HEADERS += \
    bin.h

FORMS += binconfig.ui
