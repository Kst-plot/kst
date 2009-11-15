include($$PWD/../../../../kst.pri)
include($$PWD/../../../../dataobjectplugin.pri)

TARGET = $$qtLibraryTarget(kstplugin_cumulativesum)

SOURCES += \
    cumulativesum.cpp

HEADERS += \
    cumulativesum.h

FORMS += cumulativesumconfig.ui
