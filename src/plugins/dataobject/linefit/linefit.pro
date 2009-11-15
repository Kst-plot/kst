include($$PWD/../../../../kst.pri)
include($$PWD/../../../../dataobjectplugin.pri)

TARGET = $$qtLibraryTarget(kstplugin_linefit)

SOURCES += \
    linefit.cpp

HEADERS += \
    linefit.h

FORMS += linefitconfig.ui
