include($$PWD/../../../../kst.pri)
include($$PWD/../../../../dataobjectplugin.pri)

TARGET = $$qtLibraryTarget(kstplugin_syncbin)

SOURCES += \
    syncbin.cpp

HEADERS += \
    syncbin.h

FORMS += syncbinconfig.ui
