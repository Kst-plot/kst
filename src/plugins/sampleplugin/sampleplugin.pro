include($$PWD/../../../kst.pri)
include($$PWD/../../../dataobjectplugin.pri)

TARGET = $$qtLibraryTarget(kstplugin_sampleplugin)

SOURCES += \
    sampleplugin.cpp

HEADERS += \
    sampleplugin.h

FORMS += samplepluginconfig.ui
