TOPOUT_REL=../../..
include($$PWD/$$TOPOUT_REL/kst.pri)
include($$PWD/$$TOPOUT_REL/dataobjectplugin.pri)

TARGET = $$qtLibraryTarget(kstplugin_sampleplugin)

SOURCES += \
    sampleplugin.cpp

HEADERS += \
    sampleplugin.h

FORMS += samplepluginconfig.ui
